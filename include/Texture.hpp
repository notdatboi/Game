#ifndef SPARK_TEXTURE_HPP
#define SPARK_TEXTURE_HPP

#include<Image.hpp>
#include<HardwareImageBuffer.hpp>

namespace spk
{
    class Texture
    {
    public:
        Texture();
        void setFormat(const vk::Format format);                //
        void setMipmapLevelCount(const uint32_t levelCount);         // must be called before loading, 1 means no mipmaps
        void setExtent(const vk::Extent3D extent);
        void load();
        void loadFromImage(const Image& src);
        void waitUntilReady() const;
        //Texture& copyToImage(Image& dst) const;

        void clearResources();
        ~Texture();
    private:
        friend class ShaderSet;
        const vk::Image& getImage() const;
        const vk::ImageView& getView() const;
        const vk::Sampler& getSampler() const;

        void generateMipmaps();
        void createView();
        void createSampler();
        
        HardwareImageBuffer image;
        vk::ImageView view;
        vk::Extent3D extent;
        vk::ImageUsageFlags usage;
        vk::Sampler sampler;
    };
}

#endif