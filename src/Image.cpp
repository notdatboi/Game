#include<Image.hpp>

namespace spk
{
    Image::Image()
    {
    }

    Image& Image::load(ImageLoader* loader, const std::string filename)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        loader->load(filename);
        const void* rawImageData = loader->getPtr();
        const auto imageProperties = loader->getProperties();
        extent.width = imageProperties.width;
        extent.height = imageProperties.height;
        extent.depth = 1;
        
        data.setSize(extent.width * extent.height * extent.depth)
            .setUsage(vk::BufferUsageFlagBits::eTransferSrc)
            .loadFromMemory(rawImageData);

        loader->unload();

        return *this;
    }

    void Image::clearResources()
    {
        data.clearResources();       
    }

    const vk::Buffer& Image::getData() const
    {
        return data.getVkBuffer();
    }

    vk::Buffer& Image::getData()
    {
        return data.getVkBuffer();
    }

    const vk::Extent3D Image::getExtent() const
    {
        return extent;
    }

    Image::~Image()
    {
        clearResources();
    }
}