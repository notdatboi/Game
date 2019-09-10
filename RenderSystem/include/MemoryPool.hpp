#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP
#include<System.hpp>

class MemoryPool
{
public:
    MemoryPool();
    void create(const System* system, const uint32_t memoryObjectCount);
    const VkDeviceMemory& operator[](const uint32_t index) const;
    VkDeviceMemory& operator[](const uint32_t index);
    Array<uint32_t> allocate(const uint32_t memoryObjectIndex, const VkMemoryPropertyFlags property, const Array<VkMemoryRequirements>& group);   // returns offsets
    void allocate(const uint32_t memoryObjectIndex, const VkMemoryPropertyFlags property, const VkMemoryRequirements& mem);
    void* map(const uint32_t memoryObjectIndex, const uint32_t offset, const uint32_t size);
    void flush(const uint32_t memoryObjectIndex, const uint32_t offset, const uint32_t size);
    void unmap(const uint32_t memoryObjectIndex);
    static void align(uint32_t& size, const uint32_t alignment);
    void destroy();
    ~MemoryPool();
private:
    Array<VkDeviceMemory> memory;
    const System* system;
};

#endif