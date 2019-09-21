#ifndef BUFFER_HOLDER_HPP
#define BUFFER_HOLDER_HPP
#include<System.hpp>
#include<vector>

class BufferHolder
{
public:
    BufferHolder();
    void create(const System* system);
    void addBuffers(const uint32_t count);
    void initBuffer(const uint32_t index, const VkDeviceSize size, const VkBufferUsageFlags usage);
    void addAndInitBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage);
    const size_t getCurrentBufferCount() const;
    void destroyBuffer(const uint32_t index);
    const VkMemoryRequirements getMemoryRequirements(const uint32_t index);
    void bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t bufferIndex);
    const VkBuffer& operator[](const uint32_t index) const;
    VkBuffer& operator[](const uint32_t index);
    void destroy();
    ~BufferHolder();
private:
    const System* system;
    std::vector<VkBuffer> buffers;
};

#endif