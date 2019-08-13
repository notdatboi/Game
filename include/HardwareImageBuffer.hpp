#ifndef SPARK_HARDWARE_IMAGE_BUFFER_HPP
#define SPARK_HARDWARE_IMAGE_BUFFER_HPP

#include<System.hpp>
#include<Executives.hpp>
#include<MemoryManager.hpp>

namespace spk
{
    class HardwareImageBuffer
    {
    public:
        HardwareImageBuffer();
        HardwareImageBuffer& setFormat(const vk::Format format);
        HardwareImageBuffer& setMipmapLevelCount(const uint32_t levelCount);         // must be called before loading, 1 means no mipmaps
        HardwareImageBuffer& setExtent(const vk::Extent3D extent);
        HardwareImageBuffer& setUsage(const vk::ImageUsageFlags usage);
        HardwareImageBuffer& loadFromVkBuffer(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags);    // layout must be Undefined
        HardwareImageBuffer& update(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags);                                                      // layout must be TransferDst
        HardwareImageBuffer& changeLayout(const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout, const vk::ImageSubresourceRange subresource);
        HardwareImageBuffer& blit(const vk::Image& dstImage, const vk::ImageLayout srcLayout, const vk::ImageLayout dstLayout, const vk::ImageBlit blitInfo);
        HardwareImageBuffer& waitUntilReady();
        static const std::optional<vk::Format> getSupportedFormat(const std::vector<vk::Format> formats, const vk::ImageTiling tiling, const vk::FormatFeatureFlags flags);

        const vk::Image& getVkImage() const;
        const uint32_t getMipmapLevelCount() const;
        const vk::Format getFormat() const;
        void clearResources();
        ~HardwareImageBuffer();
    private:
        void load();

        vk::CommandBuffer commands;
        vk::Image image;
        vk::Fence readyFence;
        vk::Semaphore readySemaphore;

        system::AllocatedMemoryData memoryData;
        vk::Format format;
        uint32_t levelCount;
        vk::Extent3D extent;
        vk::ImageUsageFlags usage;
    };
}
#endif