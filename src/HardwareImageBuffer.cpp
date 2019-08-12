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
    }
     
    HardwareImageBuffer& HardwareImageBuffer::setFormat(const vk::Format format)
    {
        this->format = format;
    }

    HardwareImageBuffer& HardwareImageBuffer::setMipmapLevelCount(const uint32_t levelCount)
    {
        this->levelCount = levelCount;
    }

    HardwareImageBuffer& HardwareImageBuffer::setExtent(const vk::Extent3D extent)
    {
        this->extent = extent;
    }

    HardwareImageBuffer& HardwareImageBuffer::setUsage(const vk::ImageUsageFlags usage)
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
        allocationInfo.flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        allocationInfo.memoryTypeBits = imageMemoryRequiremets.memoryTypeBits;
        allocationInfo.size = imageMemoryRequiremets.size;

        memoryData = system::MemoryManager::getInstance()->allocateMemory(allocationInfo);
        const auto& memory = system::MemoryManager::getInstance()->getMemory(memoryData.index);
        if(logicalDevice.bindImageMemory(image, memory, memoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind image memory!\n");
    }

    HardwareImageBuffer& HardwareImageBuffer::loadFromVkBuffer(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        load();
        vk::ImageSubresourceLayers subresource;
        subresource.setAspectMask(aspectFlags)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setMipLevel(0);
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
        
        if(logicalDevice.waitForFences(1, &readyFence, true, ~0U) != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for image to be ready.\n");
        logicalDevice.resetFences(1, &readyFence);

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
    }

    const vk::Image& HardwareImageBuffer::getVkImage() const
    {
        return image;
    }
}
