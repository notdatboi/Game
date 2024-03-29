#include<MemoryPool.hpp>
#include<numeric>

void MemoryPool::align(uint32_t& size, const uint32_t alignment)
{
    if(size % alignment != 0)
    {
        size = (size / alignment + 1) * alignment;
    }
}

void MemoryPool::align(uint32_t& alignment1, uint32_t& alignment2)
{
    alignment1 = alignment2 = std::lcm(alignment1, alignment2);
}

const uint32_t MemoryPool::align(const uint32_t alignment1, const uint32_t alignment2)
{
    return std::lcm(alignment1, alignment2);
}

MemoryPool::MemoryPool(){}

void MemoryPool::create(const System* system, const uint32_t memoryObjectCount)
{
    this->system = system;
    memory.create(memoryObjectCount);
}

Array<uint32_t> MemoryPool::allocate(const uint32_t memoryObjectIndex, const VkMemoryPropertyFlags property, const Array<VkMemoryRequirements>& group)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(system->getPhysicalDevice(), &memoryProperties);
    Array<uint32_t> offsets;
    offsets.create(group.getSize());
    offsets[0] = 0;
    uint32_t alignment = group[0].alignment, memoryType = group[0].memoryTypeBits, size = group[0].size, memoryTypeIndex = (~0);
    for(auto ind = 1; ind < group.getSize(); ++ind)
    {
        memoryType &= group[ind].memoryTypeBits;
        offsets[ind] = size;
        size += group[ind].size;
        if(alignment != group[ind].alignment)
        {
            reportError("Group members have different alignments.\n");
        }
    }
    if(memoryType == 0) reportError("No way to choose common memory type.\n");
    for(uint32_t ind = 0; ind < 32; ++ind)
    {
        if(((1U << ind) & memoryType) != 0)
        {
            if((memoryProperties.memoryTypes[ind].propertyFlags & property) != 0)
            {
                memoryTypeIndex = ind;
                break;
            }
        }
    }
    if(memoryTypeIndex == (~0)) reportError("No way to allocate memory with these memory properties.\n");
    VkMemoryAllocateInfo memoryInfo;
    memoryInfo = 
    {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        size,
        memoryTypeIndex
    };
    checkResult(vkAllocateMemory(system->getDevice(), &memoryInfo, nullptr, &memory[memoryObjectIndex]), "Failed to allocate memory.\n");
    return offsets;
}

void MemoryPool::allocate(const uint32_t memoryObjectIndex, const VkMemoryPropertyFlags property, const VkMemoryRequirements& mem)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(system->getPhysicalDevice(), &memoryProperties);
    uint32_t memoryTypeIndex = (~0);
    for(uint32_t ind = 0; ind < 32; ++ind)
    {
        if(((1U << ind) & mem.memoryTypeBits) != 0)
        {
            if((memoryProperties.memoryTypes[ind].propertyFlags & property) != 0)
            {
                memoryTypeIndex = ind;
                break;
            }
        }
    }
    if(memoryTypeIndex == (~0)) reportError("No way to allocate memory with this memory properties.\n");
    VkMemoryAllocateInfo memoryInfo;
    memoryInfo = 
    {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        nullptr,
        mem.size,
        memoryTypeIndex
    };
    checkResult(vkAllocateMemory(system->getDevice(), &memoryInfo, nullptr, &memory[memoryObjectIndex]), "Failed to allocate memory.\n");
}

void* MemoryPool::map(const uint32_t memoryObjectIndex, const VkDeviceSize offset, const VkDeviceSize size)
{
    void* data;
    checkResult(vkMapMemory(system->getDevice(), memory[memoryObjectIndex], offset, size, 0, &data), "Failed to map memory.\n");
    return data;
}

void MemoryPool::flush(const uint32_t memoryObjectIndex, const VkDeviceSize offset, const VkDeviceSize size)
{
    VkMappedMemoryRange range = 
    {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        nullptr,
        memory[memoryObjectIndex],
        offset,
        size
    };
    checkResult(vkFlushMappedMemoryRanges(system->getDevice(), 1, &range), "Failed to flush memory.\n");
}

void MemoryPool::unmap(const uint32_t memoryObjectIndex)
{
    vkUnmapMemory(system->getDevice(), memory[memoryObjectIndex]);
}

const VkDeviceMemory& MemoryPool::operator[](const uint32_t index) const
{
    return memory[index];
}

VkDeviceMemory& MemoryPool::operator[](const uint32_t index)
{
    return memory[index];
}

void MemoryPool::destroy()
{
    for(auto ind = 0; ind < memory.getSize(); ++ind)
    {
        if(memory[ind])
        {
            vkFreeMemory(system->getDevice(), memory[ind], nullptr);
            memory[ind] = 0;
        }
    }
}

MemoryPool::~MemoryPool()
{
    destroy();
}