#ifndef SPARK_HARDWARE_IMAGE_BUFFER_HPP
#define SPARK_HARDWARE_IMAGE_BUFFER_HPP

#include<System.hpp>
#include<Executives.hpp>
#include<MemoryManager.hpp>
#include<HardwareResource.hpp>

namespace spk
{
    class HardwareImageBuffer : public HardwareResource
    {
    public:
        HardwareImageBuffer();
        virtual void setShadowBufferPolicy(bool use = false);
        virtual void setAccessibility(const HardwareResourceAccessibility accessibility);
        void setFormat(const vk::Format format);                    // sets format, must be called before loading, does not check format availability
        void setMipmapLevelCount(const uint32_t levelCount);        // must be called before loading, 1 means no mipmaps
        void setExtent(const vk::Extent3D extent);                  // sets extent, must be called before loading
        void setUsage(const vk::ImageUsageFlags usage);             // sets usage, must be called before loading
        virtual void load();                                                
        void loadFromVkBuffer(const vk::Buffer& buffer, const vk::ImageAspectFlags aspectFlags);
        void changeLayout(/*const vk::ImageLayout oldLayout, */const vk::ImageLayout newLayout, const vk::ImageSubresourceRange subresource);
        void blit(const vk::Image& dstImage, const vk::ImageLayout srcLayout, const vk::ImageLayout dstLayout, const vk::ImageBlit blitInfo);
        virtual void waitUntilReady();
        virtual void resetWaiter();
        static const std::optional<vk::Format> getSupportedFormat(const std::vector<vk::Format> formats, const vk::ImageTiling tiling, const vk::FormatFeatureFlags flags);

        const vk::Image& getVkImage() const;
        const uint32_t getMipmapLevelCount() const;
        const vk::Format getFormat() const;
        const bool isLoaded() const;
        virtual void clearResources();
        virtual ~HardwareImageBuffer();
    private:
        bool useShadowBuffer = false;
        system::AllocatedMemoryData shadowMemoryData;
        vk::Buffer shadow;

        HardwareResourceAccessibility accessibility;

        vk::CommandBuffer commands;
        vk::Image image;
        vk::Fence readyFence;
        bool waitForSemaphore = false;
        vk::Semaphore readySemaphore;

        system::AllocatedMemoryData imageMemoryData;
        std::vector<vk::ImageLayout> subresourceLayouts;
        vk::Format format;
        uint32_t levelCount;
        vk::Extent3D extent;
        vk::ImageUsageFlags usage;

        bool loaded;
    };
}
#endif