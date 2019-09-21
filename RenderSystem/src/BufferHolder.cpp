#include<BufferHolder.hpp>

BufferHolder::BufferHolder()
{

}

void BufferHolder::create(const System* system)
{
    this->system = system;
}

void BufferHolder::destroyBuffer(const uint32_t index)
{
    if(buffers[index])
    {
        vkDestroyBuffer(system->getDevice(), buffers[index], nullptr);
        buffers[index] = 0;
    }
}

void BufferHolder::addBuffers(const uint32_t count)
{
    buffers.insert(buffers.end(), count, VkBuffer());
}

void BufferHolder::initBuffer(const uint32_t index, const VkDeviceSize size, const VkBufferUsageFlags usage)
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

void BufferHolder::addAndInitBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage)
{
    addBuffers(1);
    initBuffer(getCurrentBufferCount() - 1, size, usage);
}

const size_t BufferHolder::getCurrentBufferCount() const
{
    return buffers.size();
}

const VkMemoryRequirements BufferHolder::getMemoryRequirements(const uint32_t index)
{
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(system->getDevice(), buffers[index], &requirements);
    return requirements;
}

void BufferHolder::bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t bufferIndex)
{
    vkBindBufferMemory(system->getDevice(), buffers[bufferIndex], memory, offset);
}

const VkBuffer& BufferHolder::operator[](const uint32_t index) const
{
    return buffers[index];
}

VkBuffer& BufferHolder::operator[](const uint32_t index)
{
    return buffers[index];
}

void BufferHolder::destroy()
{
    for(auto ind = 0; ind < buffers.size(); ++ind)
    {
        destroyBuffer(ind);
    }
    buffers.clear();
}

BufferHolder::~BufferHolder()
{
    destroy();
}