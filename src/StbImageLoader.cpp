#include<StbImageLoader.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include<Stb/stb_image.h>

namespace spk
{
    StbImageLoader::StbImageLoader()
    {
    }

    void StbImageLoader::addLoadProperties(const StbLoadProperties loadProperties)
    {
        this->loadProperties = loadProperties;
    }

    void StbImageLoader::load(const std::string filename)
    {
        unload();
        int tmpW, tmpH, tmpC;
        if(loadProperties.load16Bit)
        {
            data = stbi_load_16(filename.c_str(), &tmpW, &tmpH, &tmpC, loadProperties.forceChannelCount == 255 ? 0 : loadProperties.forceChannelCount);
            properties.bytesPerChannel = 2;
        }
        else
        {
            data = stbi_load(filename.c_str(), &tmpW, &tmpH, &tmpC, loadProperties.forceChannelCount == 255 ? 0 : loadProperties.forceChannelCount);
            properties.bytesPerChannel = 1;
        }
        properties.width = tmpW;
        properties.height = tmpH;
        properties.channels = loadProperties.forceChannelCount == 255 ? tmpC : loadProperties.forceChannelCount;
    }

    const void* StbImageLoader::getPtr() const
    {
        return data;
    }

    const ImageProperties StbImageLoader::getProperties() const
    {
        return properties;
    }

    void StbImageLoader::unload()
    {
        if(data != nullptr) stbi_image_free(data);
    }

    StbImageLoader::~StbImageLoader()
    {
        unload();
    }
}