#include<ImageLoader.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include<External/Stb/stb_image.h>

ImageLoader::ImageLoader(): data(nullptr)
{
}

void ImageLoader::load(const char* filename, int& width, int& height, int& channelCount)
{
    int channels = channelCount;
    data = stbi_load(filename, &width, &height, &channels, channelCount);
    if(channelCount == 0) channelCount = channels;
}

const unsigned char* ImageLoader::getData() const
{
    return data;
}

void ImageLoader::unload()
{
    if(data)
    {
        stbi_image_free(data);
        data = nullptr;
    }
}

ImageLoader::~ImageLoader()
{
    unload();
}