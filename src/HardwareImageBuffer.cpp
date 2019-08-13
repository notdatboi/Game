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
        if(logicalDevice.createFence(&fenceInfo, nullptr, &readyFence) != vk::Result::eSuccess) throw std::runtime_error("Failed to create fence!\n");
        vk::SemaphoreCreateInfo semaphoreInfo;
        if(logicalDevice.createSemaphore(&semaphoreInfo, nullptr, &readySemaphore) != vk::Result::eSuccess) throw std::runtime_error("Failed to create semaphore!\n");
        memoryData.index = ~0;
        memoryData.offset = ~0;
    }
     
    HardwareImageBuffer& HardwareImageBuffer::setFormat(const vk::Format format)
    {
        this->format = format;

        return *this;
    }

    HardwareImageBuffer& HardwareImageBuffer::setMipmapLevelCount(const uint32_t levelCount)
    {
        this->levelCount = levelCount;

        return *this;
    }

    HardwareImageBuffer& HardwareImageBuffer::setExtent(const vk::Extent3D extent)
    {
        this->extent = extent;

        return *this;
    }

    HardwareImageBuffer& HardwareImageBuffer::setUsage(const vk::ImageUsageFlags usage)
    {
        this->usage = usage;

        return *this;
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
        allocationInfo.flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        allocationInfo.memoryTypeBits = imageMemoryRequiremets.memoryTypeBits;
        allocationInfo.size = imageMemoryRequiremets.size;

        memoryData = system::MemoryManager::getInstance()->allocateMemory(allocationInfo);
        const auto& memory = system::MemoryManager::getInstance()->getMemory(memoryData.index);
        if(logicalDevice.bindImageMemory(image, memory, memoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind image memory!\n");
    }

    HardwareImageBuffer& HardwareImageBuffer::loadFromVkBuffer(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags)
    {
        load();

        vk::ImageSubresourceRange subresourceRange;
        subresourceRange.setAspectMask(aspectFlags)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(1);

        changeLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, subresourceRange);
        update(buffer, aspectFlags);

        return *this;
    }

    HardwareImageBuffer& HardwareImageBuffer::update(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        vk::ImageSubresourceRange subresourceRange;
        subresourceRange.setAspectMask(aspectFlags)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(1);

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

        vk::CommandBufferBeginInfo beginInfo;
        commands.begin(&beginInfo);
        commands.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &copyInfo);
        commands.end();

        vk::PipelineStageFlags stageFlags = vk::PipelineStageFlagBits::eTransfer;
        vk::SubmitInfo submit;
        submit.setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setWaitSemaphoreCount(0)
            .setPWaitSemaphores(nullptr)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore)
            .setPWaitDstStageMask(&stageFlags);

        graphicsQueue.submit(1, &submit, readyFence);

        return *this;
    }

    HardwareImageBuffer& HardwareImageBuffer::changeLayout(const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout, const vk::ImageSubresourceRange subresource)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();
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
        commands.reset(vk::CommandBufferResetFlags());

        vk::CommandBufferBeginInfo beginInfo;
        commands.begin(&beginInfo);
        commands.pipelineBarrier(srcStageFlags,
            dstStageFlags,
            vk::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, &barrier);
        commands.end();

        vk::SubmitInfo submit;
        submit.setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&readySemaphore)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore)
            .setPWaitDstStageMask(&dstStageFlags);

        graphicsQueue.submit(1, &submit, readyFence);

        return *this;
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

    HardwareImageBuffer& HardwareImageBuffer::blit(const vk::Image& dstImage, const vk::ImageLayout srcLayout, const vk::ImageLayout dstLayout, const vk::ImageBlit blitInfo)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        waitUntilReady();
        commands.reset(vk::CommandBufferResetFlags());

        vk::CommandBufferBeginInfo beginInfo;
        commands.begin(&beginInfo);
        commands.blitImage(image, srcLayout, dstImage, dstLayout, 1, &blitInfo, vk::Filter::eLinear);
        commands.end();

        vk::PipelineStageFlags stageFlags = vk::PipelineStageFlagBits::eTransfer;
        vk::SubmitInfo submit;
        submit.setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(&readySemaphore)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore)
            .setPWaitDstStageMask(&stageFlags);

        graphicsQueue.submit(1, &submit, readyFence);

        return *this;
    }

    HardwareImageBuffer& HardwareImageBuffer::waitUntilReady()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(logicalDevice.waitForFences(1, &readyFence, true, ~0U) != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for image to be ready.\n");
        logicalDevice.resetFences(1, &readyFence);

        return *this;
    }

    void HardwareImageBuffer::clearResources()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        waitUntilReady();
        if(memoryData.index != (~0) && memoryData.offset != (~0))
        {
            system::MemoryManager::getInstance()->freeMemory(memoryData.index);
            memoryData.index = ~0;
            memoryData.offset = ~0;
        }
        if(image)
        {
            logicalDevice.destroyImage(image, nullptr);
            image = vk::Image();
        }
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
