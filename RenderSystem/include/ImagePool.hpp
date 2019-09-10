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
    static void recordMipmapGenCommands(VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkExtent2D& imageExtent, const uint32_t mipmapLevelCount); // first mipmap image must have oldLayout layout, and other mipmap images must have Undefined layout
    static const uint32_t getMipmapLevelCount(const VkExtent2D& imageExtent);
    static const uint32_t getMipmapLevelCount(const VkExtent3D& imageExtent);
    void create(const System* system, const uint32_t count);
    bool checkFormatSupport(const VkFormat format, const VkFormatFeatureFlags features, const VkImageTiling tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL) const;
    void createImage(const uint32_t index, const VkFormat format, const VkExtent3D& extent, const uint32_t mipmapLevels, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageAspectFlags aspect, const bool createView, const bool createSampler);
    void destroyImage(const uint32_t index);
    const VkMemoryRequirements getMemoryRequirements(const uint32_t index) const;
    void bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t imageIndex) const;
    const ImageInfo& operator[](const uint32_t index) const;
    ImageInfo& operator[](const uint32_t index);
    void destroy();
    ~ImagePool();
private:
    const System* system;
    Array<ImageInfo> images;
};

#endif