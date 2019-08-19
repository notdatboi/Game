#include<HardwareImageBuffer.hpp>

namespace spk
{
    const std::optional<vk::Format> HardwareImageBuffer::getSupportedFormat(const std::vector<vk::Format> formats, const vk::ImageTiling tiling, const vk::FormatFeatureFlags flags)
    {
        const vk::PhysicalDevice& physicalDevice = system::System::getInstance()->getPhysicalDevice();
        std::optional<vk::Format> result;
        for(const auto fmt : formats)
        {
            vk::FormatProperties properties;
            physicalDevice.getFormatProperties(fmt, &properties);
            if(tiling == vk::ImageTiling::eLinear)
            {
                if(properties.linearTilingFeatures & flags)
                {
                    result = fmt;
                    break;
                }
            }
            else
            {
                if(properties.optimalTilingFeatures & flags)
                {
                    result = fmt;
                    break;
                }
            }
        }
        return result;
    }

    HardwareImageBuffer::HardwareImageBuffer()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& commandPool = system::Executives::getInstance()->getPool();
        levelCount = 1;
        vk::CommandBufferAllocateInfo commandBufferInfo;
        commandBufferInfo.setCommandBufferCount(1)
            .setCommandPool(commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary);
        if(logicalDevice.allocateCommandBuffers(&commandBufferInfo, &commands) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate command buffer\n");
        vk::FenceCreateInfo fenceInfo;
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        if(logicalDevice.createFence(&fenceInfo, nullptr, &readyFence) != vk::Result::eSuccess) throw std::runtime_error("Failed to create fence!\n");
        vk::SemaphoreCreateInfo semaphoreInfo;
        if(logicalDevice.createSemaphore(&semaphoreInfo, nullptr, &readySemaphore) != vk::Result::eSuccess) throw std::runtime_error("Failed to create semaphore!\n");
        imageMemoryData.index = ~0;
        imageMemoryData.offset = ~0;
        shadowMemoryData.index = ~0;
        shadowMemoryData.offset = ~0;
    }

    void HardwareImageBuffer::setShadowBufferPolicy(bool use)
    {
        useShadowBuffer = use;
    }

    void HardwareImageBuffer::setAccessibility(const HardwareResourceAccessibility accessibility)
    {
        this->accessibility = accessibility;
    }

    void HardwareImageBuffer::setFormat(const vk::Format format)
    {
        this->format = format;
    }

    void HardwareImageBuffer::setMipmapLevelCount(const uint32_t levelCount)
    {
        this->levelCount = levelCount;
        subresourceLayouts = std::vector<vk::ImageLayout>(levelCount, vk::ImageLayout::eUndefined);
    }

    void HardwareImageBuffer::setExtent(const vk::Extent3D extent)
    {
        this->extent = extent;
    }

    void HardwareImageBuffer::setUsage(const vk::ImageUsageFlags usage)
    {
        this->usage = usage;
    }

    void HardwareImageBuffer::load()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};
        vk::ImageCreateInfo imageInfo;
        imageInfo.setImageType(vk::ImageType::e2D)
            .setFormat(format)
            .setExtent(extent)
            .setMipLevels(levelCount)
            .setArrayLayers(1)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setUsage(usage)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndexCount(1)
            .setPQueueFamilyIndices(queueFamIndices)
            .setInitialLayout(vk::ImageLayout::eUndefined);

        if(logicalDevice.createImage(&imageInfo, nullptr, &image) != vk::Result::eSuccess) throw std::runtime_error("Failed to create image!\n");

        vk::MemoryRequirements imageMemoryRequiremets;
        logicalDevice.getImageMemoryRequirements(image, &imageMemoryRequiremets);
        system::MemoryAllocationInfo allocationInfo;
        allocationInfo.alignment = imageMemoryRequiremets.alignment;
        if(accessibility == HardwareResourceAccessibility::Static)
        {
            allocationInfo.flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        }
        else
        {
            allocationInfo.flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        }
        allocationInfo.memoryTypeBits = imageMemoryRequiremets.memoryTypeBits;
        allocationInfo.size = imageMemoryRequiremets.size;

        imageMemoryData = system::MemoryManager::getInstance()->allocateMemory(allocationInfo);
        const auto& memory = system::MemoryManager::getInstance()->getMemory(imageMemoryData.index);
        if(logicalDevice.bindImageMemory(image, memory, imageMemoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind image memory!\n");

        if(useShadowBuffer)
        {
            vk::BufferCreateInfo shadowInfo;
            shadowInfo.setQueueFamilyIndexCount(1)
                .setPQueueFamilyIndices(queueFamIndices)
                .setSharingMode(vk::SharingMode::eExclusive)
                .setSize(extent.width * extent.height * extent.depth)
                .setUsage(vk::BufferUsageFlagBits::eTransferSrc);
            
            if(logicalDevice.createBuffer(&shadowInfo, nullptr, &shadow) != vk::Result::eSuccess) throw std::runtime_error("Failed to create buffer!\n");

            vk::MemoryRequirements shadowMemRequirements;
            logicalDevice.getBufferMemoryRequirements(shadow, &shadowMemRequirements);
            system::MemoryAllocationInfo shadowMemInfo(shadowMemRequirements);
            shadowMemInfo.flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            shadowMemoryData = system::MemoryManager::getInstance()->allocateMemory(shadowMemInfo);
            const vk::DeviceMemory& shadowMem = system::MemoryManager::getInstance()->getMemory(shadowMemoryData.index);
            if(logicalDevice.bindBufferMemory(shadow, shadowMem, shadowMemoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind memory!\n");
        }

        loaded = true;
    }

    void HardwareImageBuffer::loadFromVkBuffer(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags)
    {
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        vk::ImageSubresourceRange subresourceRange;
        subresourceRange.setAspectMask(aspectFlags)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(1);

        if(!loaded)
        {
            load();
            changeLayout(vk::ImageLayout::eTransferDstOptimal, subresourceRange);
        }

        //changeLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, subresourceRange);
        vk::ImageSubresourceLayers subresource;
        subresource.setAspectMask(aspectFlags)
            .setBaseArrayLayer(subresourceRange.baseArrayLayer)
            .setLayerCount(subresourceRange.layerCount)
            .setMipLevel(subresourceRange.baseMipLevel);
        vk::BufferImageCopy copyInfo;
        copyInfo.setBufferOffset(0)
            .setBufferRowLength(0)
            .setBufferImageHeight(0)
            .setImageExtent(extent)
            .setImageOffset({0, 0, 0})
            .setImageSubresource(subresource);
        
        waitUntilReady();
        resetWaiter();
        commands.reset(vk::CommandBufferResetFlags());

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commands.begin(&beginInfo);
        commands.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &copyInfo);
        vk::BufferCopy shadowCopyInfo;
        if(useShadowBuffer)
        {
            shadowCopyInfo.setSrcOffset(0)
                .setDstOffset(0)
                .setSize(extent.width * extent.height * extent.depth);
            commands.copyBuffer(buffer, shadow, 1, &shadowCopyInfo);
        }
        commands.end();

        vk::PipelineStageFlags stageFlags = vk::PipelineStageFlagBits::eTransfer;
        vk::SubmitInfo submit;

        if(waitForSemaphore)
        {
            submit.setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&readySemaphore);
        }
        else
        {
            submit.setWaitSemaphoreCount(0)
                .setPWaitSemaphores(nullptr);
            waitForSemaphore = true;
        }

        submit.setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore)
            .setPWaitDstStageMask(&stageFlags);

        graphicsQueue.submit(1, &submit, readyFence);
    }

    void HardwareImageBuffer::changeLayout(/*const vk::ImageLayout oldLayout, */const vk::ImageLayout newLayout, const vk::ImageSubresourceRange subresource)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        vk::ImageLayout oldLayout = subresourceLayouts[subresource.baseMipLevel];
        for(auto index = subresource.baseMipLevel; index < subresource.baseMipLevel + subresource.levelCount; ++index)
        {
            if(subresourceLayouts[index] != oldLayout) throw std::runtime_error("These subresources have different layouts.\n");
            subresourceLayouts[index] = newLayout;
        }

        vk::AccessFlags srcAccessFlags, dstAccessFlags;
        vk::PipelineStageFlags srcStageFlags, dstStageFlags;
        if(oldLayout == vk::ImageLayout::eUndefined)
        {
            srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
            if(newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
            {
                dstStageFlags = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                dstAccessFlags = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
            }
            else if(newLayout == vk::ImageLayout::eTransferDstOptimal)
            {
                dstStageFlags = vk::PipelineStageFlagBits::eTransfer;
                dstAccessFlags = vk::AccessFlagBits::eTransferWrite;
            }
            else if(newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            {
                dstStageFlags = vk::PipelineStageFlagBits::eFragmentShader;
                dstAccessFlags = vk::AccessFlagBits::eShaderRead;
            }
            else throw std::invalid_argument("Unsupported layout transition.\n");
        }
        else if(oldLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            srcStageFlags = vk::PipelineStageFlagBits::eTransfer;
            srcAccessFlags = vk::AccessFlagBits::eTransferWrite;
            if(newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
            {
                dstStageFlags = vk::PipelineStageFlagBits::eFragmentShader;
                dstAccessFlags = vk::AccessFlagBits::eShaderRead;
            }
            else throw std::invalid_argument("Unsupported layout transition.\n");
        }
        else if(oldLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            srcStageFlags = vk::PipelineStageFlagBits::eAllGraphics;
            srcAccessFlags = vk::AccessFlagBits::eShaderRead;
            if(newLayout == vk::ImageLayout::eTransferDstOptimal)
            {
                dstStageFlags = vk::PipelineStageFlagBits::eTransfer;
                dstAccessFlags = vk::AccessFlagBits::eTransferWrite;
            }
            else throw std::invalid_argument("Unsupported layout transition.\n");
        }
        else throw std::invalid_argument("Unsupported layout transition.\n");

        vk::ImageMemoryBarrier barrier;
        barrier.setSrcAccessMask(srcAccessFlags)
            .setDstAccessMask(dstAccessFlags)
            .setOldLayout(oldLayout)
            .setNewLayout(newLayout)
            .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
            .setImage(image)
            .setSubresourceRange(subresource);
        
        waitUntilReady();
        resetWaiter();
        commands.reset(vk::CommandBufferResetFlags());

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commands.begin(&beginInfo);
        commands.pipelineBarrier(srcStageFlags,
            dstStageFlags,
            vk::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, &barrier);
        commands.end();

        vk::SubmitInfo submit;

        if(waitForSemaphore)
        {
            submit.setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&readySemaphore);
        }
        else
        {
            submit.setWaitSemaphoreCount(0)
                .setPWaitSemaphores(nullptr);
            waitForSemaphore = true;
        }

        submit.setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore)
            .setPWaitDstStageMask(&dstStageFlags);

        graphicsQueue.submit(1, &submit, readyFence);
    }

    const vk::Image& HardwareImageBuffer::getVkImage() const
    {
        return image;
    }

    const uint32_t HardwareImageBuffer::getMipmapLevelCount() const
    {
        return levelCount;
    }

    const vk::Format HardwareImageBuffer::getFormat() const
    {
        return format;
    }

    const bool HardwareImageBuffer::isLoaded() const
    {
        return loaded;
    }

    void HardwareImageBuffer::blit(const vk::Image& dstImage, const vk::ImageLayout srcLayout, const vk::ImageLayout dstLayout, const vk::ImageBlit blitInfo)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        waitUntilReady();
        resetWaiter();
        commands.reset(vk::CommandBufferResetFlags());

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commands.begin(&beginInfo);
        commands.blitImage(image, srcLayout, dstImage, dstLayout, 1, &blitInfo, vk::Filter::eLinear);
        commands.end();

        vk::PipelineStageFlags stageFlags = vk::PipelineStageFlagBits::eTransfer;

        vk::SubmitInfo submit;

        if(waitForSemaphore)
        {
            submit.setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&readySemaphore);
        }
        else
        {
            submit.setWaitSemaphoreCount(0)
                .setPWaitSemaphores(nullptr);
            waitForSemaphore = true;
        }

        submit.setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore)
            .setPWaitDstStageMask(&stageFlags);

        graphicsQueue.submit(1, &submit, readyFence);
    }

    void HardwareImageBuffer::waitUntilReady() const
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(logicalDevice.waitForFences(1, &readyFence, true, ~0U) != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for image to be ready.\n");
    }

    void HardwareImageBuffer::resetWaiter()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        logicalDevice.resetFences(1, &readyFence);
    }

    void HardwareImageBuffer::clearResources()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        waitUntilReady();
        //resetWaiter();
        commands.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
        if(imageMemoryData.index != (~0) && imageMemoryData.offset != (~0))
        {
            system::MemoryManager::getInstance()->freeMemory(imageMemoryData.index);
            imageMemoryData.index = ~0;
            imageMemoryData.offset = ~0;
        }
        if(image)
        {
            logicalDevice.destroyImage(image, nullptr);
            image = vk::Image();
        }
        if(useShadowBuffer)
        {
            if(shadowMemoryData.index != (~0) && shadowMemoryData.offset != (~0))
            {
                system::MemoryManager::getInstance()->freeMemory(shadowMemoryData.index);
                shadowMemoryData.index = ~0;
                shadowMemoryData.offset = ~0;
            }
            if(shadow)
            {
                logicalDevice.destroyBuffer(shadow, nullptr);
                shadow = vk::Buffer();
            }
        }
        loaded = false;
    }

    HardwareImageBuffer::~HardwareImageBuffer()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        clearResources();
        if(readyFence)
        {
            logicalDevice.destroyFence(readyFence, nullptr);
            readyFence = vk::Fence();
        }
        if(readySemaphore)
        {
            logicalDevice.destroySemaphore(readySemaphore, nullptr);
            readySemaphore = vk::Semaphore();
        }
    }
}
