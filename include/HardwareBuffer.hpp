#ifndef SPARK_HARDWARE_BUFFER_HPP
#define SPARK_HARDWARE_BUFFER_HPP

#include<System.hpp>
#include<MemoryManager.hpp>
#include<Executives.hpp>
#include<HardwareResource.hpp>

namespace spk
{
    class HardwareBuffer : public HardwareResource
    {
    public:
        HardwareBuffer();
        virtual void setShadowBufferPolicy(bool use = false);
        virtual void setAccessibility(const HardwareResourceAccessibility accessibility);
        void setUsage(const vk::BufferUsageFlags usage);
        void setSize(const uint32_t size);
        virtual void load();
        void loadFromBuffer(const HardwareBuffer& buffer);
        void loadFromMemory(const void* data);
        virtual void waitUntilReady() const;
        virtual void resetWaiter();
        const vk::Buffer& getVkBuffer() const;
        vk::Buffer& getVkBuffer();
        virtual void clearResources();
        virtual ~HardwareBuffer();
    private:
        bool useShadowBuffer = false;
        system::AllocatedMemoryData shadowMemoryData;
        vk::Buffer shadow;

        HardwareResourceAccessibility accessibility;
        vk::BufferUsageFlags usage;
        uint32_t size;
        bool loaded;

        system::AllocatedMemoryData bufferMemoryData;
        vk::Buffer buffer;

        vk::CommandBuffer commands;
        vk::Fence readyFence;
        vk::Semaphore readySemaphore;
        bool waitForSemaphore = false;
    };
}

#endif