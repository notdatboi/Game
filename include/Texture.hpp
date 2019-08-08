#ifndef SPARK_TEXTURE_HPP
#define SPARK_TEXTURE_HPP

#include<Image.hpp>

namespace spk
{
    class Texture
    {
    public:
        Texture();
        Texture& setFormat(const vk::Format format);                //
        Texture& setMipmapLevelCount(const uint32_t levelCount);         // must be called before loading, 1 means no mipmaps
        Texture& loadFromImage(const Image& src);
        //Texture& copyToImage(Image& dst) const;
    private:
        static const std::optional<vk::Format> getSupportedFormat(const std::vector<vk::Format> formats, const vk::ImageTiling tiling, const vk::FormatFeatureFlags flags);

        vk::Image texture;
        vk::ImageView view;
        vk::ImageLayout layout;
        vk::Format format;
        system::AllocatedMemoryData memoryData;

        uint32_t mipmapLevelCount;                                       // 
        vk::CommandBuffer commands;
        vk::Semaphore textureReadySemaphore;
        vk::Fence textureReadyFence;
    };
}

#endif