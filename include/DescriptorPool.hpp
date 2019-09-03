#ifndef DESCRIPTOR_POOL_HPP
#define DESCRIPTOR_POOL_HPP
#include<System.hpp>

class DescriptorPool
{
public:
    DescriptorPool();
    void create(const System* system, const uint32_t setCount, const Array<VkDescriptorPoolSize>& poolSizes);
    void allocateSets(const uint32_t first, const Array<VkDescriptorSetLayout>& layouts);
    void updateImage(const VkDescriptorImageInfo& info, const VkDescriptorType& type, const uint32_t set, const uint32_t binding, const uint32_t arrayElement = 0, const uint32_t descriptorCount = 1);
    void updateBuffer(const VkDescriptorBufferInfo& info, const VkDescriptorType& type, const uint32_t set, const uint32_t binding, const uint32_t arrayElement = 0, const uint32_t descriptorCount = 1);
    const VkDescriptorSet& operator[](const uint32_t index) const;
    void destroy();
    ~DescriptorPool();
private:
    void updateDescriptorSets(const VkDescriptorImageInfo* img, const VkDescriptorBufferInfo* buf, const VkDescriptorType& type, const uint32_t set, const uint32_t binding, const uint32_t arrayElement = 0, const uint32_t descriptorCount = 1);
    const System* system;
    VkDescriptorPool pool;
    Array<VkDescriptorSet> sets;
};

#endif