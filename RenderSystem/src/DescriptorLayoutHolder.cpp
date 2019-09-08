#include<DescriptorLayoutHolder.hpp>

DescriptorLayoutHolder::DescriptorLayoutHolder()
{

}

void DescriptorLayoutHolder::create(const System* system, const uint32_t setLayoutCount, const uint32_t pipelineLayoutCount)
{
    this->system = system;
    setLayouts.create(setLayoutCount);
    pipelineLayouts.create(pipelineLayoutCount);
}

void DescriptorLayoutHolder::createSetLayout(const uint32_t index, const Array<VkDescriptorSetLayoutBinding>& bindings)
{
    VkDescriptorSetLayoutCreateInfo setLayoutInfo = 
    {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        bindings.getSize(),
        bindings.getPtr()
    };
    checkResult(vkCreateDescriptorSetLayout(system->getDevice(), &setLayoutInfo, nullptr, &setLayouts[index]), "Failed to create layout.\n");
}

void DescriptorLayoutHolder::createPipelineLayout(const uint32_t index, const Array<uint32_t>& setLayoutIndices)
{
    Array<VkDescriptorSetLayout> layouts;
    layouts.create(setLayoutIndices.getSize());
    for(auto ind = 0; ind < layouts.getSize(); ++ind)
    {
        layouts[ind] = setLayouts[setLayoutIndices[ind]];
    }
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = 
    {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        nullptr,
        0,
        layouts.getSize(),
        layouts.getPtr(),
        0,
        nullptr
    };
    checkResult(vkCreatePipelineLayout(system->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayouts[index]), "Failed to create layout.\n");
}

const VkPipelineLayout& DescriptorLayoutHolder::getPipelineLayout(const uint32_t index) const
{
    return pipelineLayouts[index];
}

const VkDescriptorSetLayout& DescriptorLayoutHolder::getSetLayout(const uint32_t index) const
{
    return setLayouts[index];
}

void DescriptorLayoutHolder::destroy()
{
    for(auto ind = 0; ind < pipelineLayouts.getSize(); ++ind)
    {
        if(pipelineLayouts[ind])
        {
            vkDestroyPipelineLayout(system->getDevice(), pipelineLayouts[ind], nullptr);
            pipelineLayouts[ind] = 0;
        }
    }
    for(auto ind = 0; ind < setLayouts.getSize(); ++ind)
    {
        if(setLayouts[ind])
        {
            vkDestroyDescriptorSetLayout(system->getDevice(), setLayouts[ind], nullptr);
            setLayouts[ind] = 0;
        }
    }
    pipelineLayouts.clean();
    setLayouts.clean();
}

DescriptorLayoutHolder::~DescriptorLayoutHolder()
{
    destroy();
}