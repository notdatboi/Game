#include<DescriptorPool.hpp>

DescriptorPool::DescriptorPool(){}

void DescriptorPool::create(const System* system, const uint32_t setCount, const Array<VkDescriptorPoolSize>& poolSizes)
{
    this->system = system;
    sets.create(setCount);
    VkDescriptorPoolCreateInfo poolInfo = 
    {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        nullptr,
        0,
        setCount,
        poolSizes.getSize(),
        poolSizes.getPtr()
    };
    checkResult(vkCreateDescriptorPool(system->getDevice(), &poolInfo, nullptr, &pool), "Failed to create descriptor pool.\n");
}

void DescriptorPool::allocateSets(const uint32_t first, const Array<VkDescriptorSetLayout>& layouts)
{
    VkDescriptorSetAllocateInfo allocInfo = 
    {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        nullptr,
        pool,
        layouts.getSize(),
        layouts.getPtr()
    };
    checkResult(vkAllocateDescriptorSets(system->getDevice(), &allocInfo, &sets[first]), "Failed to allocate descriptor sets.\n");
}

void DescriptorPool::updateDescriptorSets(const VkDescriptorImageInfo* img, const VkDescriptorBufferInfo* buf, const VkDescriptorType type, const uint32_t set, const uint32_t binding, const uint32_t arrayElement, const uint32_t descriptorCount)
{
    VkWriteDescriptorSet write = 
    {
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        nullptr,
        sets[set],
        binding,
        arrayElement,
        descriptorCount,
        type,
        img,
        buf,
        nullptr
    };
    vkUpdateDescriptorSets(system->getDevice(), 1, &write, 0, nullptr);
}

void DescriptorPool::updateImage(const VkDescriptorImageInfo& info, const VkDescriptorType type, const uint32_t set, const uint32_t binding, const uint32_t arrayElement, const uint32_t descriptorCount)
{
    updateDescriptorSets(&info, nullptr, type, set, binding, arrayElement, descriptorCount);
}

void DescriptorPool::updateBuffer(const VkDescriptorBufferInfo& info, const VkDescriptorType type, const uint32_t set, const uint32_t binding, const uint32_t arrayElement, const uint32_t descriptorCount)
{
    updateDescriptorSets(nullptr, &info, type, set, binding, arrayElement, descriptorCount);
}

const VkDescriptorSet& DescriptorPool::operator[](const uint32_t index) const
{
    return sets[index];
}

void DescriptorPool::destroy()
{
    if(pool)
    {
        vkDestroyDescriptorPool(system->getDevice(), pool, nullptr);
        pool = 0;
    }
    sets.clear();
}

DescriptorPool::~DescriptorPool()
{
    destroy();
}