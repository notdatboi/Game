#ifndef IMAGE_HOLDER_HPP
#define IMAGE_HOLDER_HPP
#include<System.hpp>
#include<vector>

class ImageHolder
{
public:
    struct ImageData
    {
        VkImage image;
        VkImageView view;
        VkSampler sampler;
    };
    ImageHolder();
    static void recordLayouChangeCommands(const VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkImageSubresourceRange& subresource);
    static void recordMipmapGenCommands(const VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkExtent2D& imageExtent, const uint32_t mipmapLevelCount, const VkImageLayout mipmapImageLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED); // first mipmap image must have oldLayout layout, and other mipmap images must have Undefined layout
    static const uint32_t getMipmapLevelCount(const VkExtent2D& imageExtent);
    static const uint32_t getMipmapLevelCount(const VkExtent3D& imageExtent);
    void create(const System* system);
    bool checkFormatSupport(const VkFormat format, const VkFormatFeatureFlags features, const VkImageTiling tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL) const;
    void addImages(const uint32_t count);
    void initImage(const uint32_t index, const VkFormat format, const VkExtent3D& extent, const uint32_t mipmapLevels, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageAspectFlags aspect, const bool createView, const bool createSampler);
    void addAndInitImage(const VkFormat format, const VkExtent3D& extent, const uint32_t mipmapLevels, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkImageAspectFlags aspect, const bool createView, const bool createSampler);
    const size_t getCurrentImageCount() const;
    void destroyImage(const uint32_t index);
    const VkMemoryRequirements getMemoryRequirements(const uint32_t index) const;
    void bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t imageIndex) const;
    const ImageData& operator[](const uint32_t index) const;
    ImageData& operator[](const uint32_t index);
    void destroy();
    ~ImageHolder();
private:
    const System* system;
    std::vector<ImageData> images;
};

#endif