#ifndef SPARK_IMAGE_LOADER_HPP
#define SPARK_IMAGE_LOADER_HPP
#include<string>

namespace spk
{
    struct ImageProperties
    {
        size_t width;
        size_t height;
        size_t channels;
    };

    class ImageLoader
    {
    public:
        virtual void load(const std::string filename) = 0;
        virtual const void* getPtr() const = 0;
        virtual const ImageProperties getProperties() const = 0;
        virtual void unload() = 0;
        virtual ~ImageLoader();
    private:
    };
}

#endif