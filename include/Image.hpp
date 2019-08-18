#ifndef SPARK_IMAGE_HPP
#define SPARK_IMAGE_HPP
#include<SparkIncludeBase.hpp>
#include<optional>
#include<string>
#include<vector>
#include<ImageLoader.hpp>
#include<HardwareBuffer.hpp>

namespace spk
{
    class Image
    {
    public:
        Image();
        Image& load(ImageLoader* loader, const std::string filename);
        void clearResources();
        const vk::Extent3D getExtent() const;
        ~Image();
    private:
        friend class Texture;
        const vk::Buffer& getData() const;
        vk::Buffer& getData();

        HardwareBuffer data;
        vk::Extent3D extent;
        system::AllocatedMemoryData memoryData;
    };
}

#endif