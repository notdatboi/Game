#ifndef SPARK_HARDWARE_BUFFER_HPP
#define SPARK_HARDWARE_BUFFER_HPP

#include<System.hpp>
#include<MemoryManager.hpp>
#include<Executives.hpp>

namespace spk
{
    //enum class HardwareBufferAccessibility
    //{
    //    Static,                             // Recommended. Assumption that the buffer is changed rarely, changing takes more time, but reading from it is greatly more optimal. Created with 'shadow buffer' to easily retrieve data
    //    Dynamic                             // Assumption that the buffer is changed frequently, changing takes almost no time, but usage takes more time
    //};

    class HardwareBuffer
    {
    public:
        HardwareBuffer();
        //HardwareBuffer& setAccessibility(const HardwareBufferAccessibility accessibility);
        HardwareBuffer& setUsage(const vk::BufferUsageFlags usage);
        HardwareBuffer& setSize(const uint32_t size);
        HardwareBuffer& load();
        //HardwareBuffer& loadFromBuffer(const HardwareBuffer& buffer);
        HardwareBuffer& loadFromMemory(const void* data);
        const vk::Buffer& getVkBuffer() const;
        vk::Buffer& getVkBuffer();
        void clearResources();
        ~HardwareBuffer();
    private:
        //HardwareBufferAccessibility accessibility;
        vk::BufferUsageFlags usage;
        uint32_t size;
        bool loaded;

        system::AllocatedMemoryData memoryData;
        vk::Buffer buffer;
        //vk::Buffer shadow;

        //vk::CommandBuffer commands;
    };
}

#endif