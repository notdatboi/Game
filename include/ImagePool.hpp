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
    static void recordLayouChangeCommands(VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkImageSubresourceRange& subresource);
    static const uint32_t getMipmapLevelCount(const VkExtent2D& imageExtent);
    void create(const System* system, const uint32_t count);
    bool checkFormatSupport(const VkFormat format, const VkFormatFeatureFlags features, const VkImageTiling tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL) const;
    void createImage(const VkFormat format, const VkExtent3D& extent, const uint32_t mipmapLevels, const VkImageUsageFlags usage, const VkImageAspectFlags aspect, const bool createView, const bool createSampler, const uint32_t index);
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