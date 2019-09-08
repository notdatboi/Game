#ifndef BUFFER_POOL_HPP
#define BUFFER_POOL_HPP
#include<System.hpp>

class BufferPool
{
public:
    BufferPool();
    void create(const System* system, const uint32_t count);
    void createBuffer(const uint32_t index, const VkDeviceSize size, const VkBufferUsageFlags usage);
    void destroyBuffer(const uint32_t index);
    const VkMemoryRequirements getMemoryRequirements(const uint32_t index);
    void bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t bufferIndex);
    const VkBuffer& operator[](const uint32_t index) const;
    VkBuffer& operator[](const uint32_t index);
    void destroy();
    ~BufferPool();
private:
    const System* system;
    Array<VkBuffer> buffers;
};

#endif