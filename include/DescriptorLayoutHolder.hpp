#ifndef DESCRIPTOR_LAYOUT_HOLDER_HPP
#define DESCRIPTOR_LAYOUT_HOLDER_HPP
#include<System.hpp>

class DescriptorLayoutHolder
{
public:
    DescriptorLayoutHolder();
    void create(const System* system, const uint32_t setLayoutCount, const uint32_t pipelineLayoutCount);
    void createSetLayout(const uint32_t index, const Array<VkDescriptorSetLayoutBinding>& bindings);
    void createPipelineLayout(const uint32_t index, const Array<uint32_t>& setLayoutIndices);
    const VkPipelineLayout& getPipelineLayout(const uint32_t index) const;
    const VkDescriptorSetLayout& getSetLayout(const uint32_t index) const;
    void destroy();
    ~DescriptorLayoutHolder();
private:
    const System* system;
    Array<VkDescriptorSetLayout> setLayouts;
    Array<VkPipelineLayout> pipelineLayouts;
};

#endif