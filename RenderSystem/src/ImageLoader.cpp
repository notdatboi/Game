#include<ImageLoader.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include<External/Stb/stb_image.h>

ImageLoader::ImageLoader()
{
}

void ImageLoader::create(const uint32_t maxImageCount)
{
    images.create(maxImageCount);
}

ImageLoader::Image& ImageLoader::operator[](const uint32_t index)
{
    return images[index];
}

const ImageLoader::Image& ImageLoader::operator[](const uint32_t index) const
{
    return images[index];
}

ImageLoader::~ImageLoader()
{
}


ImageLoader::Image::Image(){}

void ImageLoader::Image::load(const char* filename, int forcedChannels)
{
    data = stbi_load(filename, &width, &height, &channels, forcedChannels);
    if(forcedChannels != 0) channels = forcedChannels;
}

ImageLoader::Image& ImageLoader::Image::operator=(Image&& img)
{
    width = img.width;
    height = img.height;
    channels = img.channels;
    data = img.data;
    img.data = nullptr;
    return *this;
}

const unsigned char* ImageLoader::Image::getData() const
{
    return data;
}

const VkExtent2D ImageLoader::Image::getExtent() const
{
    return {width, height};
}

const uint32_t ImageLoader::Image::getChannelCount() const
{
    return channels;
}

void ImageLoader::Image::unload()
{
    if(data)
    {
        stbi_image_free(data);
        data = nullptr;
    }
}

ImageLoader::Image::~Image()
{
    unload();
}