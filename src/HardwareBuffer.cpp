#include<HardwareBuffer.hpp>

namespace spk
{
    HardwareBuffer::HardwareBuffer()
    {
        memoryData.index = (~0);
        memoryData.offset = (~0);
    }

//    HardwareBuffer& HardwareBuffer::setAccessibility(const HardwareBufferAccessibility accessibility)
//    {
//        this->accessibility = accessibility;
//    }

    HardwareBuffer& HardwareBuffer::setUsage(const vk::BufferUsageFlags usage)
    {
        this->usage = usage;
    }

    HardwareBuffer& HardwareBuffer::setSize(const uint32_t size)
    {
        this->size = size;
    }

    HardwareBuffer& HardwareBuffer::load()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.setQueueFamilyIndexCount(1)
            .setPQueueFamilyIndices(queueFamIndices)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSize(size)
            .setUsage(usage);
        
        if(logicalDevice.createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) throw std::runtime_error("Failed to create buffer!\n");

        vk::MemoryRequirements bufferRequirements;
        logicalDevice.getBufferMemoryRequirements(buffer, &bufferRequirements);

        system::MemoryAllocationInfo memoryAllocationInfo;
        memoryAllocationInfo.flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        memoryAllocationInfo.size = bufferRequirements.size;
        memoryAllocationInfo.memoryTypeBits = bufferRequirements.memoryTypeBits;
        memoryAllocationInfo.alignment = bufferRequirements.alignment;

        memoryData = system::MemoryManager::getInstance()->allocateMemory(memoryAllocationInfo);
        const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(memoryData.index);
        if(logicalDevice.bindBufferMemory(buffer, memory, memoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind buffer memory!\n");

        loaded = true;

        return *this;
    }

//    HardwareBuffer& HardwareBuffer::loadFromBuffer(const HardwareBuffer& buffer){}

    HardwareBuffer& HardwareBuffer::loadFromMemory(const void* data)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        if(!loaded)
        {
            load();
        }

        const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(memoryData.index);
        void* mapped;
        if(logicalDevice.mapMemory(memory, memoryData.offset, size, vk::MemoryMapFlags(), &mapped) != vk::Result::eSuccess) throw std::runtime_error("Failed to map memory!\n");
        memcpy(mapped, data, size);
        logicalDevice.unmapMemory(memory);

        return *this;
    }

    const vk::Buffer& HardwareBuffer::getVkBuffer() const
    {
        return buffer;
    }

    vk::Buffer& HardwareBuffer::getVkBuffer()
    {
        return buffer;
    }

    void HardwareBuffer::clearResources()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(memoryData.index != (~0) && memoryData.offset != (~0))
        {
            system::MemoryManager::getInstance()->freeMemory(memoryData.index);
            memoryData.index = (~0);
            memoryData.offset = (~0);
        }
        if(buffer)
        {
            logicalDevice.destroyBuffer(buffer, nullptr);
            buffer = vk::Buffer();
        }
    }

    HardwareBuffer::~HardwareBuffer()
    {
        clearResources();
    }
}