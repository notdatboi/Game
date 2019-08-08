#ifndef SPARK_IMAGE_HPP
#define SPARK_IMAGE_HPP
#include<SparkIncludeBase.hpp>
#include<System.hpp>
#include<optional>
#include<string>
#include<vector>
#include<MemoryManager.hpp>
#include<ImageLoader.hpp>
#include<Executives.hpp>

namespace spk
{
    class Image
    {
    public:
        Image();
        void load(ImageLoader* loader, const std::string filename);
        void clearResources();
        ~Image();
    private:
        friend class Texture;
        const vk::Buffer& getData() const;
        vk::Buffer& getData();
        const vk::Extent3D getExtent() const;

        vk::Buffer imageData;
        vk::Extent3D extent;
        system::AllocatedMemoryData memoryData;
    };
}

#endif