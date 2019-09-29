#ifndef IMAGE_HOLDER_HPP
#define IMAGE_HOLDER_HPP
#include<System.hpp>
#include<vector>

class ImageHolder
{
public:
    ImageHolder();
    static void recordLayoutChangeCommands(const VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkImageSubresourceRange& subresource);
    static void recordMipmapGenCommands(const VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkExtent2D& imageExtent, const uint32_t mipmapLevelCount, const VkImageLayout mipmapImageLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED); // first mipmap image must have oldLayout layout, and other mipmap images must have Undefined layout
    void create(const System* system);
    bool checkFormatSupport(const VkFormat format, const VkFormatFeatureFlags features, const VkImageTiling tiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL) const;
    void addImages(const uint32_t count);
    void addViews(const uint32_t count);
    void addSamplers(const uint32_t count);
    void initImage(const uint32_t index, 
        const VkImageCreateFlags flags,
        const VkImageType type, 
        const VkFormat format, 
        const VkExtent3D& extent, 
        const bool enableMipmapping,
        uint32_t& mipmapLevels,                 // if mipmapping is enabled, rewrites with mipmap level count
        const VkSampleCountFlagBits samples,
        const VkImageTiling tiling, 
        const VkImageUsageFlags usage);
    void initView(const uint32_t index, const uint32_t imageIndex, const VkImageViewType type, const VkFormat format, const VkImageSubresourceRange& subresource);
    void initSampler(const uint32_t index, const float minLod, const float maxLod);
    const size_t getCurrentImageCount() const;
    const size_t getCurrentViewCount() const;
    const size_t getCurrentSamplerCount() const;
    void destroyImage(const uint32_t index);
    void destroyView(const uint32_t index);
    void destroySampler(const uint32_t index);
    const VkMemoryRequirements getMemoryRequirements(const uint32_t imageIndex) const;
    void bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t imageIndex) const;
    const VkImage& getImage(const uint32_t index) const;
    const VkImageView& getView(const uint32_t index) const;
    const VkSampler& getSampler(const uint32_t index) const;
    void destroy();
    ~ImageHolder();
private:
    static const uint32_t getMipmapLevelCount(const VkExtent2D& imageExtent);
    static const uint32_t getMipmapLevelCount(const VkExtent3D& imageExtent);
    const System* system;
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
    std::vector<VkSampler> samplers;
};

struct ImageInfo
{
    uint32_t imageIndex;
    uint32_t viewIndex;
    const ImageHolder* holder;
    VkImageLayout layout;
    uint32_t mipmapLevelCount;
};

struct SampledImageInfo
{
    ImageInfo image;
    uint32_t samplerIndex;
};

#endif