#include<ObjectManagementStrategy.hpp>
#include<memory.h>

void SharedMemoryObjectManagementStrategy::create(const System* system, SynchronizationPool* syncPool, CommandPool* commandPool)
{
    this->system = system;
    this->syncPool = syncPool;
    this->commandPool = commandPool;
    createDescriptorLayouts();
    preloadDescriptorSets();
    updateFence = syncPool->getFenceCount();
    syncPool->addFences(1, true);
    updateSemaphore = syncPool->getSemaphoreCount();
    syncPool->addSemaphores(1);
    updateCommandBuffer = commandPool->getCurrentPoolSize();
    commandPool->addCommandBuffers(1);
    commandPool->allocateCommandBuffers(updateCommandBuffer, 1, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    memoryPool.create(system, MemoryObjects::Count);
    bufferHolder.create(system);
    bufferHolder.addBuffers(Buffers::Count);
    imageHolder.create(system);
    allocateTransferBuffer();
}

void SharedMemoryObjectManagementStrategy::createDescriptorLayouts()
{
    descriptorLayoutHolder.create(system, DescriptorLayouts::Count, DrawableType::Count);
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
    descriptorLayoutHolder.createSetLayout(DescriptorLayouts::SampledImageFrag, sampledFragBindings);
    descriptorLayoutHolder.createSetLayout(DescriptorLayouts::UniformFrag, uniformFragBindings);
    descriptorLayoutHolder.createSetLayout(DescriptorLayouts::UniformVertTeseGeom, uniformVertTeseGeomBindings);
    Array<uint32_t> notTexturedSetLayouts = 
    {
        DescriptorLayouts::UniformVertTeseGeom,             // view n' projection
        DescriptorLayouts::UniformVertTeseGeom,             // model
        DescriptorLayouts::UniformFrag                      // material colors
    };
    Array<uint32_t> texturedSetLayouts = 
    {
        DescriptorLayouts::UniformVertTeseGeom,             // view & projection
        DescriptorLayouts::UniformVertTeseGeom,             // model
        DescriptorLayouts::UniformFrag,                     // mat colors
        DescriptorLayouts::SampledImageFrag                 // texture
    };
    Array<uint32_t> texturedWithNormalMapSetLayouts = 
    {
        DescriptorLayouts::UniformVertTeseGeom,             // view and projection
        DescriptorLayouts::UniformVertTeseGeom,             // model
        DescriptorLayouts::UniformFrag,                     // material colors
        DescriptorLayouts::SampledImageFrag,                // texture
        DescriptorLayouts::SampledImageFrag                 // normal map
    };
    descriptorLayoutHolder.createPipelineLayout(DrawableType::NotTextured, notTexturedSetLayouts);
    descriptorLayoutHolder.createPipelineLayout(DrawableType::Textured, texturedSetLayouts);
    descriptorLayoutHolder.createPipelineLayout(DrawableType::TexturedWithNormalMap, texturedWithNormalMapSetLayouts);
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
    uint32_t size = std::max(MAX_IMAGE_DIMENSION * MAX_IMAGE_DIMENSION, MAX_VERTEX_SIZE * MAX_VERTEX_COUNT);
    bufferHolder.initBuffer(Buffers::Transfer, size, usage);
    memoryPool.allocate(MemoryObjects::Transfer, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, bufferHolder.getMemoryRequirements(Buffers::Transfer));
    bufferHolder.bindMemory(memoryPool[MemoryObjects::Transfer], 0, Buffers::Transfer);
    mappedTransferMemory = memoryPool.map(MemoryObjects::Transfer, 0, size);
}

void SharedMemoryObjectManagementStrategy::allocateSampledImage(const VkExtent3D& extent, SampledImageInfo& sampledImage, DescriptorInfo& sampledImageDescriptor)
{
    const uint32_t index = imageHolder.getCurrentImageCount();
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
    pickImageFormat(format, tiling);
    imageHolder.addAndInitImage(format, extent, ImageHolder::getMipmapLevelCount(extent), tiling, usage, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT, true, true);
    sampledImage.image.image = &imageHolder[index].image;
    sampledImage.image.view = &imageHolder[index].view;
    sampledImage.sampler = &imageHolder[index].sampler;
    memoryRequirements[MemoryObjects::Image].push_back(imageHolder.getMemoryRequirements(index));
    sampledImage.image.layout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;

    // descriptor creation

    Array<VkDescriptorSetLayout> layouts = {descriptorLayoutHolder.getSetLayout(DescriptorLayouts::SampledImageFrag)};
    descriptorPool.allocateSets(currentDescriptorCount, layouts);
    sampledImageDescriptor.set = &descriptorPool[currentDescriptorCount];
    sampledImageDescriptor.binding = 0;
    sampledImageDescriptor.arrayElement = 0;

    // descriptor update command creation

    VkDescriptorImageInfo info = 
    {
        *sampledImage.sampler,
        *sampledImage.image.view,
        VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };
    descriptorPool.updateImage(info, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, currentDescriptorCount, sampledImageDescriptor.binding, sampledImageDescriptor.arrayElement);
    ++currentDescriptorCount;
}

void SharedMemoryObjectManagementStrategy::allocateDepthMap(const VkExtent2D& extent, ImageInfo& depthMap)
{
    const uint32_t index = imageHolder.getCurrentImageCount();
    VkFormat format;
    VkImageTiling tiling;
    VkExtent3D extent3d = {extent.width, extent.height, 1};
    VkImageUsageFlags usage = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    pickDepthStencilFormat(format, tiling);
    imageHolder.addAndInitImage(format, extent3d, 1, tiling, usage, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT, true, false);
    depthMap.image = &imageHolder[index].image;
    depthMap.view = &imageHolder[index].view;
    memoryRequirements[MemoryObjects::Image].push_back(imageHolder.getMemoryRequirements(index));

    const auto& commands = (*commandPool)[updateCommandBuffer];
    syncPool->waitForFences(updateFence);
    syncPool->resetFences(updateFence);
    VkCommandBufferBeginInfo beginInfo = 
    {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        nullptr
    };
    VkImageSubresourceRange subresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT,
        0,
        1,
        0,
        1
    };
    if(!firstCommandBufferRun) vkResetCommandBuffer(commands, VkCommandBufferResetFlags());
    checkResult(vkBeginCommandBuffer(commands, &beginInfo), "Failed to begin command buffer");
    ImageHolder::recordLayouChangeCommands(commands, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, *depthMap.image, subresource);
    vkEndCommandBuffer(commands);

    VkPipelineStageFlags stages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submitInfo = 
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        firstCommandBufferRun ? 0 : 1,
        firstCommandBufferRun ? nullptr : &syncPool->getSemaphore(updateSemaphore),
        firstCommandBufferRun ? nullptr : &stages,
        1,
        &commands,
        1,
        &syncPool->getSemaphore(updateSemaphore)
    };
    if(firstCommandBufferRun) firstCommandBufferRun = false;
    checkResult(vkQueueSubmit(system->getGraphicsQueue().queue, 1, &submitInfo, syncPool->getFence(updateFence)), "Failed to submit queue.\n");

    depthMap.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
}

void SharedMemoryObjectManagementStrategy::allocateVertexBuffer(const uint32_t size, BufferInfo& buffer)
{
    buffer.buffer = &bufferHolder[Buffers::Vertex];
    buffer.offset = vertexBufferSize;
    buffer.size = size;
    vertexBufferSize += size;
}

void SharedMemoryObjectManagementStrategy::allocateIndexBuffer(const uint32_t size, BufferInfo& buffer)
{
    buffer.buffer = &bufferHolder[Buffers::Index];
    buffer.offset = indexBufferSize;
    buffer.size = size;
    indexBufferSize += size;
}

void SharedMemoryObjectManagementStrategy::allocateUniformBuffer(const uint32_t size, const VkShaderStageFlags stages, BufferInfo& buffer, DescriptorInfo& uniformDescriptor)
{
    buffer.buffer = &bufferHolder[Buffers::Uniform];
    buffer.offset = uniformBufferSize;
    buffer.size = size;
    uniformBufferSize += size;

    // descriptor creation

    Array<VkDescriptorSetLayout> layouts;
    if(stages == VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        layouts = {descriptorLayoutHolder.getSetLayout(DescriptorLayouts::UniformFrag)};
    }
    else if(stages == VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT)
    {
        layouts = {descriptorLayoutHolder.getSetLayout(DescriptorLayouts::UniformVertTeseGeom)};
    }
    else reportError("Not supported uniform type.\n");
    descriptorPool.allocateSets(currentDescriptorCount, layouts);
    uniformDescriptor.set = &descriptorPool[currentDescriptorCount];
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
    // creating and binding image memory

    uint32_t imageAlignment = 1;
    std::vector<uint32_t> imageOffsets(memoryRequirements[MemoryObjects::Image].size());
    auto index = 0;
    uint32_t currSize = 0;
    for(const auto& req : memoryRequirements[MemoryObjects::Image])
    {
        imageAlignment = MemoryPool::align((const uint32_t)imageAlignment, req.alignment);
    }
    for(auto& req : memoryRequirements[MemoryObjects::Image])
    {   
        req.alignment = imageAlignment;
        imageOffsets[index] = currSize;
        if(req.size % req.alignment != 0)
        {
            currSize = (req.size / imageAlignment + 1) * imageAlignment;
        }
        else currSize += req.size;
        ++index;
    }
    const Array<VkMemoryRequirements> imageGroup(memoryRequirements[MemoryObjects::Image]);
    memoryPool.allocate(MemoryObjects::Image, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, imageGroup);
    for(auto ind = 0; ind < imageHolder.getCurrentImageCount(); ++ind)
    {
        imageHolder.bindMemory(memoryPool[MemoryObjects::Image], imageOffsets[ind], ind);
    }

    // creating buffers and binding memory

    bufferHolder.initBuffer(Buffers::Vertex, vertexBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    bufferHolder.initBuffer(Buffers::Index, indexBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    bufferHolder.initBuffer(Buffers::Uniform, uniformBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    uint32_t bufferAlignment = 1;
    Array<uint32_t> bufferOffsets(3);
    currSize = 0;
    Array<VkMemoryRequirements> bufferMemoryRequirements({bufferHolder.getMemoryRequirements(Buffers::Vertex), bufferHolder.getMemoryRequirements(Buffers::Index), bufferHolder.getMemoryRequirements(Buffers::Uniform)});
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
            currSize = (bufferMemoryRequirements[ind].size / bufferAlignment + 1)  * bufferAlignment;
        }
        else currSize += bufferMemoryRequirements[ind].size;
    }
    memoryPool.allocate(MemoryObjects::Buffer, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferMemoryRequirements);
    bufferHolder.bindMemory(memoryPool[MemoryObjects::Buffer], bufferOffsets[0], Buffers::Vertex);
    bufferHolder.bindMemory(memoryPool[MemoryObjects::Buffer], bufferOffsets[1], Buffers::Index);
    bufferHolder.bindMemory(memoryPool[MemoryObjects::Buffer], bufferOffsets[2], Buffers::Uniform);

    // binding descriptors to buffers
    
    for(auto& command : bufferDescriptorUpdateCommands)
    {
        VkDescriptorBufferInfo bufferInfo = 
        {
            *command.buffer->buffer,
            command.buffer->offset,
            command.buffer->size
        };
        descriptorPool.updateBuffer(bufferInfo, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, command.set, command.binding, command.arrayElement);
    }

    // cleaning up

    for(auto ind = 0; ind < MemoryObjects::Count; ++ind)
    {
        memoryRequirements[ind].clear();
    }
    bufferDescriptorUpdateCommands.clear();
}

void SharedMemoryObjectManagementStrategy::update()
{
    const auto& commands = (*commandPool)[updateCommandBuffer];

    for(auto& cmd : bufferUpdateCommands)
    {
        memcpy(mappedTransferMemory, cmd.src, cmd.dst->size);
        memoryPool.flush(MemoryObjects::Transfer, 0, cmd.dst->size);
        syncPool->waitForFences(updateFence);
        syncPool->resetFences(updateFence);

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
        if(!firstCommandBufferRun) vkResetCommandBuffer(commands, VkCommandBufferResetFlags());
        checkResult(vkBeginCommandBuffer(commands, &beginInfo), "Failed to begin command buffer");
        vkCmdCopyBuffer(commands, bufferHolder[Buffers::Transfer], *(cmd.dst->buffer), 1, &copyArea);
        vkEndCommandBuffer(commands);

        VkPipelineStageFlags stages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;        
        VkSubmitInfo submitInfo = 
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            firstCommandBufferRun ? 0 : 1,
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

    for(auto& cmd : imageUpdateCommands)
    {
        auto extent = cmd.src->getExtent();
        auto size = extent.width * extent.height * cmd.src->getChannelCount();
        memcpy(mappedTransferMemory, cmd.src->getData(), size);
        memoryPool.flush(MemoryObjects::Transfer, 0, size);
        syncPool->waitForFences(updateFence);
        syncPool->resetFences(updateFence);

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
        if(!firstCommandBufferRun) vkResetCommandBuffer(commands, VkCommandBufferResetFlags());
        checkResult(vkBeginCommandBuffer(commands, &beginInfo), "Failed to begin command buffer");
        ImageHolder::recordLayouChangeCommands(commands, cmd.dst->layout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, *(cmd.dst->image), subresourceRange);
        vkCmdCopyBufferToImage(commands, bufferHolder[Buffers::Transfer], *(cmd.dst->image), VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &area);
        ImageHolder::recordMipmapGenCommands(commands, 
            VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            *(cmd.dst->image),
            extent,
            ImageHolder::getMipmapLevelCount(extent),
            cmd.dst->layout);
        vkEndCommandBuffer(commands);

        VkPipelineStageFlags stages = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;        
        VkSubmitInfo submitInfo = 
        {
            VK_STRUCTURE_TYPE_SUBMIT_INFO,
            nullptr,
            firstCommandBufferRun ? 0 : 1,
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
}

void SharedMemoryObjectManagementStrategy::destroy()
{
    syncPool->waitForFences(updateFence);
    syncPool->resetFences(updateFence);
    memoryPool.unmap(MemoryObjects::Transfer);
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