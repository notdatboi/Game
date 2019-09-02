#ifndef IMAGE_POOL_HPP
#define IMAGE_POOL_HPP
#include<System.hpp>

struct ImageInfo
{
    VkImage image;
    VkImageView view;
    VkSampler sampler;
};

class ImagePool
{
public:
    ImagePool();
    void create(const System* system, const uint32_t count);
    bool checkFormatSupport(const VkFormat& format, const VkFormatFeatureFlags& features, const VkImageTiling& tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL) const;
    void createImage(const VkFormat& format, const VkExtent3D& extent, const bool useMipmapping, const VkImageUsageFlags& usage, const VkImageAspectFlags& aspect, const bool createView, const bool createSampler, const uint32_t index);
    const VkMemoryRequirements getMemoryRequirements(const uint32_t index);
    void bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t imageIndex);
    const ImageInfo& operator[](const uint32_t index) const;
    ImageInfo& operator[](const uint32_t index);
    void destroy();
    ~ImagePool();
private:
    const System* system;
    Array<ImageInfo> images;
};

#endif