#ifndef SPARK_STB_IMAGE_LOADER_HPP
#define SPARK_STB_IMAGE_LOADER_HPP
#include<ImageLoader.hpp>

namespace spk
{
    struct StbLoadProperties      // default values are ignored
    {
        StbLoadProperties() : forceChannelCount(255), load16Bit(false) {}
        unsigned char forceChannelCount;
        bool load16Bit;
    };

    class StbImageLoader : public ImageLoader
    {
    public:
        StbImageLoader();
        void addLoadProperties(const StbLoadProperties loadProperties);
        void load(const std::string filename);
        const void* getPtr() const;
        const ImageProperties getProperties() const;
        void unload();
        ~StbImageLoader();
    private:
        void* data;
        ImageProperties properties;
        StbLoadProperties loadProperties;
    };
}

#endif