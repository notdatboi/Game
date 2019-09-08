#ifndef IMAGE_LOADER_HPP
#define IMAGE_LOADER_HPP

class ImageLoader
{
public:
    ImageLoader();
    void load(const char* filename, int& width, int& height, int& channelCount);        // 0 channels -> auto-check
    const unsigned char* getData() const;
    void unload();
    ~ImageLoader();
private:
    unsigned char* data;
};

#endif