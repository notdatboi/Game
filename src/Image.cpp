#include<Image.hpp>

namespace spk
{
    Image::Image() : memoryData({~0, ~0})
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

        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.setQueueFamilyIndexCount(1)
            .setPQueueFamilyIndices(queueFamIndices)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSize(imageProperties.width * imageProperties.height * imageProperties.channels * imageProperties.bytesPerChannel)
            .setUsage(vk::BufferUsageFlagBits::eTransferSrc);
        
        if(logicalDevice.createBuffer(&bufferInfo, nullptr, &imageData) != vk::Result::eSuccess) throw std::runtime_error("Failed to create buffer!\n");

        vk::MemoryRequirements bufferRequirements;
        logicalDevice.getBufferMemoryRequirements(imageData, &bufferRequirements);

        system::MemoryAllocationInfo memoryAllocationInfo;
        memoryAllocationInfo.flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        memoryAllocationInfo.size = bufferRequirements.size;
        memoryAllocationInfo.memoryTypeBits = bufferRequirements.memoryTypeBits;
        memoryAllocationInfo.alignment = bufferRequirements.alignment;

        system::MemoryManager::getInstance()->allocateMemory(memoryAllocationInfo);
        const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(memoryData.index);
        if(logicalDevice.bindBufferMemory(imageData, memory, memoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind buffer memory!\n");
        void* mappedMemory;
        if(logicalDevice.mapMemory(memory, memoryData.offset, bufferInfo.size, vk::MemoryMapFlags(), &mappedMemory) != vk::Result::eSuccess) throw std::runtime_error("Failed to map memory!\n");
        memcpy(mappedMemory, rawImageData, bufferInfo.size);
        logicalDevice.unmapMemory(memory);

        loader->unload();

        return *this;
    }

    void Image::clearResources()
    {
        if(memoryData.index != (~0) && memoryData.offset != (~0))
        {
            system::MemoryManager::getInstance()->freeMemory(memoryData.index);
            memoryData = {~0, ~0};
        }
        if(imageData)
        {
            const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
            logicalDevice.destroyBuffer(imageData, nullptr);
            imageData = vk::Buffer();
        }
    }

    const vk::Buffer& Image::getData() const
    {
        return imageData;
    }

    vk::Buffer& Image::getData()
    {
        return imageData;
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