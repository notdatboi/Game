#include<Texture.hpp>

namespace spk
{
    const std::optional<vk::Format> Texture::getSupportedFormat(const std::vector<vk::Format> formats, const vk::ImageTiling tiling, const vk::FormatFeatureFlags flags)
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

    Texture& Texture::setMipmapLevelCount(const uint32_t levelCount)
    {
        mipmapLevelCount = levelCount;
    }

    Texture::Texture()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& commandPool = system::Executives::getInstance()->getPool();
        if(getSupportedFormat({vk::Format::eR8G8B8A8Unorm}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eTransferDst | vk::FormatFeatureFlagBits::eSampledImage).has_value()) format = vk::Format::eR8G8B8A8Unorm;
        layout = vk::ImageLayout::eUndefined;
        mipmapLevelCount = 1;
        vk::CommandBufferAllocateInfo commandBufferInfo;
        commandBufferInfo.setCommandBufferCount(1)
            .setCommandPool(commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary);
        if(logicalDevice.allocateCommandBuffers(&commandBufferInfo, &commands) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate command buffer\n");
        vk::FenceCreateInfo fenceInfo;
        if(logicalDevice.createFence(&fenceInfo, nullptr, &textureReadyFence) != vk::Result::eSuccess) throw std::runtime_error("Failed to create fence!\n");
        vk::SemaphoreCreateInfo semaphoreInfo;
        if(logicalDevice.createSemaphore(&semaphoreInfo, nullptr, &textureReadySemaphore) != vk::Result::eSuccess) throw std::runtime_error("Failed to create semaphore!\n");
    }

    Texture& Texture::setFormat(const vk::Format format)
    {
        vk::FormatFeatureFlags neededFormatFeatures = vk::FormatFeatureFlagBits::eTransferDst | vk::FormatFeatureFlagBits::eSampledImage;
        if(mipmapLevelCount != 1)
        {
            neededFormatFeatures |= vk::FormatFeatureFlagBits::eBlitSrc;
            neededFormatFeatures |= vk::FormatFeatureFlagBits::eBlitDst;
        }
        if(getSupportedFormat({format}, vk::ImageTiling::eOptimal, neededFormatFeatures).has_value()) this->format = format;
        else throw std::invalid_argument("Invalid or not supported image format.\n");
    }

    Texture& Texture::loadFromImage(const Image& src)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};

        vk::ImageCreateInfo imageInfo;
        imageInfo.setImageType(vk::ImageType::e2D)
            .setFormat(format)
            .setExtent(src.getExtent())
            .setMipLevels(mipmapLevelCount)
            .setArrayLayers(1)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setTiling(vk::ImageTiling::eOptimal)
            .setUsage(vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndexCount(1)
            .setPQueueFamilyIndices(queueFamIndices)
            .setInitialLayout(vk::ImageLayout::eUndefined);

        if(logicalDevice.createImage(&imageInfo, nullptr, &texture) != vk::Result::eSuccess) throw std::runtime_error("Failed to create image!\n");

        vk::MemoryRequirements textureMemoryRequiremets;
        logicalDevice.getImageMemoryRequirements(texture, &textureMemoryRequiremets);
        system::MemoryAllocationInfo allocationInfo;
        allocationInfo.alignment = textureMemoryRequiremets.alignment;
        allocationInfo.flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        allocationInfo.memoryTypeBits = textureMemoryRequiremets.memoryTypeBits;
        allocationInfo.size = textureMemoryRequiremets.size;

        memoryData = system::MemoryManager::getInstance()->allocateMemory(allocationInfo);
        const auto& memory = system::MemoryManager::getInstance()->getMemory(memoryData.index);
        if(logicalDevice.bindImageMemory(texture, memory, memoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind image memory!\n");

    }

}