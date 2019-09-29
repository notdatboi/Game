#include<ObjectManagementStrategy.hpp>
#include<memory.h>

SharedMemoryObjectManagementStrategy::SharedMemoryObjectManagementStrategy(){}

void SharedMemoryObjectManagementStrategy::create(const System* system, SynchronizationPool* syncPool, CommandPool* commandPool)
{
    this->system = system;
    this->syncPool = syncPool;
    this->commandPool = commandPool;
    vkGetPhysicalDeviceProperties(system->getPhysicalDevice(), &deviceProperties);
    createDescriptorLayouts();
    preloadDescriptorSets();
    updateFence = syncPool->getFenceCount();
    syncPool->addFences(1, true);
    updateSemaphore = syncPool->getSemaphoreCount();
    syncPool->addSemaphores(1);
    updateCommandBuffer = commandPool->getCurrentPoolSize();
    commandPool->addCommandBuffers(1);
    commandPool->allocateCommandBuffers(updateCommandBuffer, 1, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    memoryPool.create(system, MemoryObjects::MOCount);
    bufferHolder.create(system);
    bufferHolder.addBuffers(Buffers::BCount);
    imageHolder.create(system);
    allocateTransferBuffer();
}

void SharedMemoryObjectManagementStrategy::createDescriptorLayouts()
{
    descriptorLayoutHolder.create(system, DescriptorLayouts::DLCount, DrawableType::DTCount);
    VkDescriptorSetLayoutBinding sampledFragBinding = 
    {
        0,
        VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        1,
        VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT,
        nullptr
    }, uniformFragBinding = 
    {
        0,
        VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        1,
        VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT,
        nullptr
    }, uniformVertTeseGeomBinding = 
    {
        0,
        VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        1,
        VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT,
        nullptr
    };
    Array<VkDescriptorSetLayoutBinding> sampledFragBindings = {sampledFragBinding}, uniformFragBindings = {uniformFragBinding}, uniformVertTeseGeomBindings = {uniformVertTeseGeomBinding};
    descriptorLayoutHolder.createSetLayout(DescriptorLayouts::DLSampledImageFrag, sampledFragBindings);
    descriptorLayoutHolder.createSetLayout(DescriptorLayouts::DLUniformFrag, uniformFragBindings);
    descriptorLayoutHolder.createSetLayout(DescriptorLayouts::DLUniformVertTeseGeom, uniformVertTeseGeomBindings);
    Array<uint32_t> notTexturedSetLayouts = 
    {
        DescriptorLayouts::DLUniformVertTeseGeom,             // view n' projection
        DescriptorLayouts::DLUniformVertTeseGeom,             // model
        DescriptorLayouts::DLUniformFrag                      // material colors
    };
    Array<uint32_t> texturedSetLayouts = 
    {
        DescriptorLayouts::DLUniformVertTeseGeom,             // view & projection
        DescriptorLayouts::DLUniformVertTeseGeom,             // model
        DescriptorLayouts::DLUniformFrag,                     // mat colors
        DescriptorLayouts::DLSampledImageFrag                 // texture
    };
    Array<uint32_t> texturedWithNormalMapSetLayouts = 
    {
        DescriptorLayouts::DLUniformVertTeseGeom,             // view and projection
        DescriptorLayouts::DLUniformVertTeseGeom,             // model
        DescriptorLayouts::DLUniformFrag,                     // material colors
        DescriptorLayouts::DLSampledImageFrag,                // texture
        DescriptorLayouts::DLSampledImageFrag                 // normal map
    };
    descriptorLayoutHolder.createPipelineLayout(DrawableType::DTNotTextured, notTexturedSetLayouts);
    descriptorLayoutHolder.createPipelineLayout(DrawableType::DTTextured, texturedSetLayouts);
    descriptorLayoutHolder.createPipelineLayout(DrawableType::DTTexturedWithNormalMap, texturedWithNormalMapSetLayouts);
}

void SharedMemoryObjectManagementStrategy::preloadDescriptorSets()
{
    Array<VkDescriptorPoolSize> poolSizes(2);
    poolSizes[0].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = MAX_UNIFORM_COUNT;
    poolSizes[1].type = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = MAX_TEXTURE_COUNT;
    descriptorPool.create(system, MAX_TEXTURE_COUNT + MAX_UNIFORM_COUNT, poolSizes);
}

void SharedMemoryObjectManagementStrategy::pickDepthStencilFormat(VkFormat& format, VkImageTiling& tiling) const
{
    static VkFormat savedFormat = VkFormat::VK_FORMAT_UNDEFINED;
    static VkImageTiling savedTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
    if(savedFormat != VkFormat::VK_FORMAT_UNDEFINED)
    {
        format = savedFormat;
        tiling = savedTiling;
        return;
    }
    for(const auto& fmt : {VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT, VkFormat::VK_FORMAT_D24_UNORM_S8_UINT, VkFormat::VK_FORMAT_D16_UNORM_S8_UINT})
    {
        if(imageHolder.checkFormatSupport(fmt, VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
        {
            savedFormat = fmt;
            savedTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
            break;
        }
        else if(imageHolder.checkFormatSupport(fmt, VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VkImageTiling::VK_IMAGE_TILING_LINEAR))
        {
            savedFormat = fmt;
            savedTiling = VkImageTiling::VK_IMAGE_TILING_LINEAR;
            break;
        }
    }
    if(savedFormat == VkFormat::VK_FORMAT_UNDEFINED) reportError("No supported depth/stencil image formats.\n");
    format = savedFormat;
    tiling = savedTiling;
}

void SharedMemoryObjectManagementStrategy::pickImageFormat(VkFormat& format, VkImageTiling& tiling) const
{
    static VkFormat savedFormat = VkFormat::VK_FORMAT_UNDEFINED;
    static VkImageTiling savedTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
    if(savedFormat != VkFormat::VK_FORMAT_UNDEFINED)
    {
        format = savedFormat;
        tiling = savedTiling;
        return;
    }
    VkFormatFeatureFlags features = VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_BLIT_SRC_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_BLIT_DST_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_TRANSFER_SRC_BIT | VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    for(const auto& fmt : {VkFormat::VK_FORMAT_R8G8B8A8_UNORM})
    {
        if(imageHolder.checkFormatSupport(fmt, features))
        {
            savedFormat = fmt;
            savedTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
            break;
        }
        else if(imageHolder.checkFormatSupport(fmt, features, VkImageTiling::VK_IMAGE_TILING_LINEAR))
        {
            savedFormat = fmt;
            savedTiling = VkImageTiling::VK_IMAGE_TILING_LINEAR;
            break;
        }
    }
    if(savedFormat == VkFormat::VK_FORMAT_UNDEFINED) reportError("No supported color image formats.\n");
    format = savedFormat;
    tiling = savedTiling;
}

void SharedMemoryObjectManagementStrategy::allocateTransferBuffer()
{
    VkBufferUsageFlags usage = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    uint32_t size = std::max(MAX_IMAGE_DIMENSION * MAX_IMAGE_DIMENSION * 4, MAX_VERTEX_SIZE * MAX_VERTEX_COUNT);
    const VkDeviceSize& alignment = deviceProperties.limits.minMemoryMapAlignment;
    size = (size % alignment != 0) ? (size / alignment + 1) * alignment : size;
    bufferHolder.initBuffer(Buffers::BTransfer, size, usage);
    memoryPool.allocate(MemoryObjects::MOTransfer, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT /*temporarily | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT*/, bufferHolder.getMemoryRequirements(Buffers::BTransfer));
    bufferHolder.bindMemory(memoryPool[MemoryObjects::MOTransfer], 0, Buffers::BTransfer);
    mappedTransferMemory = memoryPool.map(MemoryObjects::MOTransfer, 0, size);
}

void SharedMemoryObjectManagementStrategy::allocateSampledImage(const VkExtent3D& extent, SampledImageInfo& sampledImage, DescriptorInfo& sampledImageDescriptor)
{
    const uint32_t index = imageHolder.getCurrentImageCount(), viewIndex = imageHolder.getCurrentViewCount(), samplerIndex = imageHolder.getCurrentSamplerCount();
    uint32_t mipmapLevels;
    VkFormat format;
    VkImageTiling tiling;
    VkImageType type = VkImageType::VK_IMAGE_TYPE_2D;
    VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    pickImageFormat(format, tiling);
    imageHolder.addImages(1);
    imageHolder.addViews(1);
    imageHolder.addSamplers(1);
    imageHolder.initImage(index, 0, type, format, extent, true, mipmapLevels, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, tiling, usage);
    imageHolder.initSampler(samplerIndex, 0, mipmapLevels - 1);
    sampledImage.image.holder = &imageHolder;
    sampledImage.image.imageIndex = index;
    sampledImage.image.viewIndex = viewIndex;
    sampledImage.image.mipmapLevelCount = mipmapLevels;
    sampledImage.samplerIndex = samplerIndex;
    sampledImage.image.layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageSubresourceRange subresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        mipmapLevels,
        0,
        1
    };
    ViewCreateCommand viewCmd = 
    {
        viewIndex,
        index,
        VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
        format,
        subresource       
    };
    memoryRequirements[MemoryObjects::MOImage].push_back(imageHolder.getMemoryRequirements(index));
    viewCreateCommands.push_back(viewCmd);

    InitialImageLayoutUpdateCommand layoutUpdateCmd = 
    {
        &sampledImage.image,
        VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        subresource
    };
    layoutUpdateCommands.push_back(layoutUpdateCmd);

    // descriptor creation

    Array<VkDescriptorSetLayout> layouts = {descriptorLayoutHolder.getSetLayout(DescriptorLayouts::DLSampledImageFrag)};
    descriptorPool.allocateSets(currentDescriptorCount, layouts);
    sampledImageDescriptor.pool = &descriptorPool;
    sampledImageDescriptor.setIndex = currentDescriptorCount;
    sampledImageDescriptor.binding = 0;
    sampledImageDescriptor.arrayElement = 0;

    // descriptor update command creation

    ImageDescriptorUpdateCommand descriptorUpdateCmd = 
    {
        &sampledImage,
        VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        currentDescriptorCount,
        sampledImageDescriptor.binding,
        sampledImageDescriptor.arrayElement,
        1
    };
    imageDescriptorUpdateCommands.push_back(descriptorUpdateCmd);
    ++currentDescriptorCount;
}

void SharedMemoryObjectManagementStrategy::allocateDepthMap(const VkExtent2D& extent, ImageInfo& depthMap)
{
    const uint32_t imgIndex = imageHolder.getCurrentImageCount(), viewIndex = imageHolder.getCurrentViewCount();
    uint32_t mLevels;
    VkFormat format;
    VkImageTiling tiling;
    VkExtent3D extent3d = {extent.width, extent.height, 1};
    VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    pickDepthStencilFormat(format, tiling);
    imageHolder.addImages(1);
    imageHolder.initImage(imgIndex, 0, VkImageType::VK_IMAGE_TYPE_2D, format, extent3d, false, mLevels, VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, tiling, usage);
    imageHolder.addViews(1);
    depthMap.holder = &imageHolder;
    depthMap.imageIndex = imgIndex;
    depthMap.viewIndex = viewIndex;
    depthMap.mipmapLevelCount = 1;
    depthMap.layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageSubresourceRange subresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT,
        0,
        1,
        0,
        1
    };
    ViewCreateCommand viewCmd = 
    {
        viewIndex,
        imgIndex,
        VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
        format,
        subresource
    };
    memoryRequirements[MemoryObjects::MOImage].push_back(imageHolder.getMemoryRequirements(imgIndex));
    viewCreateCommands.push_back(viewCmd);

    InitialImageLayoutUpdateCommand layoutUpdateCmd = 
    {
        &depthMap,
        VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        subresource
    };
    layoutUpdateCommands.push_back(layoutUpdateCmd);
}

void SharedMemoryObjectManagementStrategy::allocateVertexBuffer(const uint32_t size, BufferInfo& buffer)
{
    buffer.index = Buffers::BVertex;
    buffer.holder = &bufferHolder;
    buffer.offset = vertexBufferSize;
    buffer.size = size;
    vertexBufferSize += size;
    const VkDeviceSize& alignment = deviceProperties.limits.minStorageBufferOffsetAlignment;
    vertexBufferSize = vertexBufferSize % alignment != 0 ? (vertexBufferSize / alignment + 1) * alignment : vertexBufferSize;
}

void SharedMemoryObjectManagementStrategy::allocateIndexBuffer(const uint32_t size, BufferInfo& buffer)
{
    buffer.index = Buffers::BIndex;
    buffer.holder = &bufferHolder;
    buffer.offset = indexBufferSize;
    buffer.size = size;
    indexBufferSize += size;
    const VkDeviceSize& alignment = deviceProperties.limits.minStorageBufferOffsetAlignment;
    indexBufferSize = indexBufferSize % alignment != 0 ? (indexBufferSize / alignment + 1) * alignment : indexBufferSize;
}

void SharedMemoryObjectManagementStrategy::allocateUniformBuffer(const uint32_t size, const VkShaderStageFlags stages, BufferInfo& buffer, DescriptorInfo& uniformDescriptor)
{
    buffer.holder = &bufferHolder;
    buffer.index = Buffers::BUniform;
    buffer.offset = uniformBufferSize;
    buffer.size = size;
    uniformBufferSize += size;
    const VkDeviceSize& alignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
    uniformBufferSize = uniformBufferSize % alignment != 0 ? (uniformBufferSize / alignment + 1) * alignment : uniformBufferSize;

    // descriptor creation

    Array<VkDescriptorSetLayout> layouts;
    if(stages == VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        layouts = {descriptorLayoutHolder.getSetLayout(DescriptorLayouts::DLUniformFrag)};
    }
    else if(stages == VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT)
    {
        layouts = {descriptorLayoutHolder.getSetLayout(DescriptorLayouts::DLUniformVertTeseGeom)};
    }
    else reportError("Not supported uniform type.\n");
    descriptorPool.allocateSets(currentDescriptorCount, layouts);
    uniformDescriptor.pool = &descriptorPool;
    uniformDescriptor.setIndex = currentDescriptorCount;
    uniformDescriptor.binding = 0;
    uniformDescriptor.arrayElement = 0;

    // descriptor update command creation

    BufferDescriptorUpdateCommand descriptorCommand = 
    {
        &buffer,
        currentDescriptorCount,
        uniformDescriptor.binding,
        uniformDescriptor.arrayElement
    };
    bufferDescriptorUpdateCommands.push_back(descriptorCommand);
    ++currentDescriptorCount;
}

void SharedMemoryObjectManagementStrategy::updateBuffer(const void* src, const BufferInfo& dst)
{
    bufferUpdateCommands.push_back({src, &dst});
}

void SharedMemoryObjectManagementStrategy::updateImage(const ImageLoader::Image& src, const ImageInfo& dst)
{
    imageUpdateCommands.push_back({&src, &dst});
}

const VkPipelineLayout& SharedMemoryObjectManagementStrategy::getPipelineLayout(const DrawableType type)
{
    return descriptorLayoutHolder.getPipelineLayout(type);
}

void SharedMemoryObjectManagementStrategy::load()
{
    // creating and binding image memory; creating image views; initializing image descriptors

    uint32_t imageAlignment = 1;
    std::vector<uint32_t> imageOffsets(memoryRequirements[MemoryObjects::MOImage].size());
    auto index = 0;
    uint32_t currSize = 0;
    uint32_t memoryTypeBits = ~0U;
    for(const auto& req : memoryRequirements[MemoryObjects::MOImage])
    {
        imageAlignment = MemoryPool::align((const uint32_t)imageAlignment, req.alignment);
        memoryTypeBits &= req.memoryTypeBits;
    }
    for(auto& req : memoryRequirements[MemoryObjects::MOImage])
    {   
        req.alignment = imageAlignment;
        imageOffsets[index] = currSize;
        if(req.size % req.alignment != 0)
        {
            currSize += (req.size / imageAlignment + 1) * imageAlignment;
        }
        else currSize += req.size;
        ++index;
    }
    if(memoryTypeBits == 0) reportError("Couldn't allocate all images in single memory object.\n");
    //const Array<VkMemoryRequirements> imageGroup(memoryRequirements[MemoryObjects::MOImage]);
    VkMemoryRequirements imageMemoryObjectRequirements;
    imageMemoryObjectRequirements.alignment = imageAlignment;
    imageMemoryObjectRequirements.memoryTypeBits = memoryTypeBits;
    imageMemoryObjectRequirements.size = currSize;
    memoryPool.allocate(MemoryObjects::MOImage, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageMemoryObjectRequirements);
    for(auto ind = 0; ind < imageHolder.getCurrentImageCount(); ++ind)
    {
        imageHolder.bindMemory(memoryPool[MemoryObjects::MOImage], imageOffsets[ind], ind);
    }
    for(const auto& viewCreateCommand : viewCreateCommands)
    {
        imageHolder.initView(viewCreateCommand.index, viewCreateCommand.imageIndex, viewCreateCommand.type, viewCreateCommand.format, viewCreateCommand.subresource);
    }

    const auto& updateCmd = (*commandPool)[updateCommandBuffer];
    for(auto& cmd : layoutUpdateCommands)
    {
        syncPool->waitForFences(updateFence);
        syncPool->resetFences(updateFence);
        VkCommandBufferBeginInfo beginInfo = 
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            nullptr,
            VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            nullptr
        };
        if(!firstCommandBufferRun) commandPool->reset(updateCommandBuffer, true);
        checkResult(vkBeginCommandBuffer(updateCmd, &beginInfo), "Failed to begin command buffer");
        ImageHolder::recordLayoutChangeCommands(updateCmd, cmd.image->layout, cmd.newLayout, cmd.image->holder->getImage(cmd.image->imageIndex), cmd.subresource);
        vkEndCommandBuffer(updateCmd);

        VkPipelineStageFlags stages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkSubmitInfo submitInfo = 
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            firstCommandBufferRun ? 0U : 1U,
            firstCommandBufferRun ? nullptr : &syncPool->getSemaphore(updateSemaphore),
            firstCommandBufferRun ? nullptr : &stages,
            1,
            &updateCmd,
            1,
            &syncPool->getSemaphore(updateSemaphore)
        };
        if(firstCommandBufferRun) firstCommandBufferRun = false;
        checkResult(vkQueueSubmit(system->getGraphicsQueue().queue, 1, &submitInfo, syncPool->getFence(updateFence)), "Failed to submit queue.\n");
        cmd.image->layout = cmd.newLayout;
    }

    for(const auto& cmd : imageDescriptorUpdateCommands)
    {
        VkDescriptorImageInfo info = 
        {
            cmd.image->image.holder->getSampler(cmd.image->samplerIndex),
            cmd.image->image.holder->getView(cmd.image->image.viewIndex),
            cmd.image->image.layout
        };
        descriptorPool.updateImage(info, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, cmd.set, cmd.binding, cmd.arrayElement, cmd.descriptorCount);
    }
    
    // creating buffers and binding memory

    bufferHolder.initBuffer(Buffers::BVertex, vertexBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT /* DEBUG ALERT */ | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    bufferHolder.initBuffer(Buffers::BIndex, indexBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT /* DEBUG ALERT */ | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    bufferHolder.initBuffer(Buffers::BUniform, uniformBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT /* DEBUG ALERT */ | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    uint32_t bufferAlignment = 1;
    Array<uint32_t> bufferOffsets(3);
    currSize = 0;
    Array<VkMemoryRequirements> bufferMemoryRequirements({bufferHolder.getMemoryRequirements(Buffers::BVertex), bufferHolder.getMemoryRequirements(Buffers::BIndex), bufferHolder.getMemoryRequirements(Buffers::BUniform)});
    for(auto ind = 0; ind < bufferMemoryRequirements.getSize(); ++ind)
    {
        bufferAlignment = MemoryPool::align(bufferMemoryRequirements[ind].alignment, (const uint32_t)bufferAlignment);
    }
    for(auto ind = 0; ind < bufferMemoryRequirements.getSize(); ++ind)
    {
        bufferMemoryRequirements[ind].alignment = bufferAlignment;
        bufferOffsets[ind] = currSize;
        if(bufferMemoryRequirements[ind].size % bufferAlignment != 0)
        {
            currSize += (bufferMemoryRequirements[ind].size / bufferAlignment + 1)  * bufferAlignment;
        }
        else currSize += bufferMemoryRequirements[ind].size;
    }
    memoryPool.allocate(MemoryObjects::MOBuffer, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferMemoryRequirements);
    bufferHolder.bindMemory(memoryPool[MemoryObjects::MOBuffer], bufferOffsets[0], Buffers::BVertex);
    bufferHolder.bindMemory(memoryPool[MemoryObjects::MOBuffer], bufferOffsets[1], Buffers::BIndex);
    bufferHolder.bindMemory(memoryPool[MemoryObjects::MOBuffer], bufferOffsets[2], Buffers::BUniform);

    // binding descriptors to buffers
    
    for(auto& command : bufferDescriptorUpdateCommands)
    {
        VkDescriptorBufferInfo bufferInfo = 
        {
            (*command.buffer->holder)[command.buffer->index],
            command.buffer->offset,
            command.buffer->size
        };
        descriptorPool.updateBuffer(bufferInfo, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, command.set, command.binding, command.arrayElement);
    }

    // cleaning up

    for(auto ind = 0; ind < MemoryObjects::MOCount; ++ind)
    {
        memoryRequirements[ind].clear();
    }
    bufferDescriptorUpdateCommands.clear();
    layoutUpdateCommands.clear();
    imageDescriptorUpdateCommands.clear();
}

void SharedMemoryObjectManagementStrategy::update()
{
    const auto& commands = (*commandPool)[updateCommandBuffer];

    for(auto& cmd : bufferUpdateCommands)
    {
        syncPool->waitForFences(updateFence);
        syncPool->resetFences(updateFence);
        memcpy(mappedTransferMemory, cmd.src, cmd.dst->size);
        memoryPool.flush(MemoryObjects::MOTransfer, 0, VK_WHOLE_SIZE);

        VkCommandBufferBeginInfo beginInfo = 
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            nullptr,
            VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            nullptr
        };
        VkBufferCopy copyArea = 
        {
            0,
            cmd.dst->offset,
            cmd.dst->size
        };
        if(!firstCommandBufferRun) commandPool->reset(updateCommandBuffer, true);
        checkResult(vkBeginCommandBuffer(commands, &beginInfo), "Failed to begin command buffer");
        vkCmdCopyBuffer(commands, bufferHolder[Buffers::BTransfer], (*cmd.dst->holder)[cmd.dst->index], 1, &copyArea);
        vkEndCommandBuffer(commands);

        VkPipelineStageFlags stages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;        
        VkSubmitInfo submitInfo = 
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            firstCommandBufferRun ? 0U : 1U,
            firstCommandBufferRun ? nullptr : &syncPool->getSemaphore(updateSemaphore),
            firstCommandBufferRun ? nullptr : &stages,
            1,
            &commands,
            1,
            &syncPool->getSemaphore(updateSemaphore)
        };
        if(firstCommandBufferRun) firstCommandBufferRun = false;

        checkResult(vkQueueSubmit(system->getGraphicsQueue().queue, 1, &submitInfo, syncPool->getFence(updateFence)), "Failed to submit queue.\n");
    }
    bufferUpdateCommands.clear();
    
    for(auto& cmd : imageUpdateCommands)
    {
        syncPool->waitForFences(updateFence);
        syncPool->resetFences(updateFence);
        auto extent = cmd.src->getExtent();
        auto size = extent.width * extent.height * cmd.src->getChannelCount();
        memcpy(mappedTransferMemory, cmd.src->getData(), size);
        memoryPool.flush(MemoryObjects::MOTransfer, 0, size);

        VkCommandBufferBeginInfo beginInfo = 
        {
            VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            nullptr,
            VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            nullptr
        };
        VkImageSubresourceLayers subresourceLayers = 
        {
            VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            0,
            1
        };
        VkImageSubresourceRange subresourceRange = 
        {
            VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
            0,
            1,
            0,
            1
        };
        VkBufferImageCopy area = 
        {
            0,
            0,
            0,
            subresourceLayers,
            {0, 0, 0},
            {extent.width, extent.height, 1}
        };

        const VkImage& currentImage = cmd.dst->holder->getImage(cmd.dst->imageIndex);

        if(!firstCommandBufferRun)  commandPool->reset(updateCommandBuffer, true);
        checkResult(vkBeginCommandBuffer(commands, &beginInfo), "Failed to begin command buffer");
        ImageHolder::recordLayoutChangeCommands(commands, cmd.dst->layout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, currentImage, subresourceRange);
        vkCmdCopyBufferToImage(commands, bufferHolder[Buffers::BTransfer], currentImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &area);
        ImageHolder::recordMipmapGenCommands(commands, 
            VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            currentImage,
            extent,
            cmd.dst->mipmapLevelCount,
            cmd.dst->layout);
        vkEndCommandBuffer(commands);

        VkPipelineStageFlags stages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;        
        VkSubmitInfo submitInfo = 
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            firstCommandBufferRun ? 0U : 1U,
            firstCommandBufferRun ? nullptr : &syncPool->getSemaphore(updateSemaphore),
            firstCommandBufferRun ? nullptr : &stages,
            1,
            &commands,
            1,
            &syncPool->getSemaphore(updateSemaphore)
        };
        if(firstCommandBufferRun) firstCommandBufferRun = false;

        checkResult(vkQueueSubmit(system->getGraphicsQueue().queue, 1, &submitInfo, syncPool->getFence(updateFence)), "Failed to submit queue.\n");
    }
    imageUpdateCommands.clear();
}

void SharedMemoryObjectManagementStrategy::destroy()
{
    vkDeviceWaitIdle(system->getDevice());
    if(mappedTransferMemory != nullptr)
    {
        memoryPool.unmap(MemoryObjects::MOTransfer);
        mappedTransferMemory = nullptr;
    }
    descriptorPool.destroy();
    descriptorLayoutHolder.destroy();
    bufferHolder.destroy();
    imageHolder.destroy();
    memoryPool.destroy();
    bufferDescriptorUpdateCommands.clear();
    bufferUpdateCommands.clear();
    imageUpdateCommands.clear();
}

SharedMemoryObjectManagementStrategy::~SharedMemoryObjectManagementStrategy()
{
    destroy();
}