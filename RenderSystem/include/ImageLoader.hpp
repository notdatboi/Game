#ifndef IMAGE_LOADER_HPP
#define IMAGE_LOADER_HPP
#include<Utils.hpp>

class ImageLoader
{
public:
    class Image
    {
    public:
        Image();
        void load(const char* filename, int forcedChannels = 0);        // 0 channels -> auto-check
        Image& operator=(Image&& img);
        Image& operator=(const Image& img) = delete;
        Image& operator=(Image& img) = delete;
        Image& operator=(Image img) = delete;
        const unsigned char* getData() const;
        const VkExtent2D getExtent() const;
        const uint32_t getChannelCount() const;
        void unload();
        ~Image();
    private:
        int width;
        int height;
        int channels;
        unsigned char* data;
    };
    ImageLoader();
    void create(const uint32_t maxImageCount);
    Image& operator[](const uint32_t index);
    const Image& operator[](const uint32_t index) const;
    ~ImageLoader();
private:
    //unsigned char* data;
    Array<Image> images;
};

#endif