#include<BufferPool.hpp>

BufferPool::BufferPool()
{

}

void BufferPool::create(const System* system, const uint32_t count)
{
    this->system = system;
    buffers.create(count);
}

void BufferPool::createBuffer(const VkDeviceSize& size, const VkBufferUsageFlags& usage, const uint32_t index)
{
    VkBufferCreateInfo bufferInfo = 
    {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        0,
        size,
        usage,
        VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
        1,
        &(system->getGraphicsQueue().familyIndex)
    };
    checkResult(vkCreateBuffer(system->getDevice(), &bufferInfo, nullptr, &buffers[index]), "Failed to create buffer.\n");
}

const VkMemoryRequirements BufferPool::getMemoryRequirements(const uint32_t index)
{
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(system->getDevice(), buffers[index], &requirements);
    return requirements;
}

void BufferPool::bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t bufferIndex)
{
    vkBindBufferMemory(system->getDevice(), buffers[bufferIndex], memory, offset);
}

const VkBuffer& BufferPool::operator[](const uint32_t index) const
{
    return buffers[index];
}

VkBuffer& BufferPool::operator[](const uint32_t index)
{
    return buffers[index];
}

void BufferPool::destroy()
{
    for(auto ind = 0; ind < buffers.getSize(); ++ind)
    {
        if(buffers[ind])
        {
            vkDestroyBuffer(system->getDevice(), buffers[ind], nullptr);
            buffers[ind] = 0;
        }
    }
}

BufferPool::~BufferPool()
{
    destroy();
}