#include"../include/DescriptorPool.hpp"

namespace spk
{
    DescriptorPool::DescriptorPool(){}

    DescriptorPool::DescriptorPool(const uint32_t maxSetCount, const std::vector<vk::DescriptorPoolSize>& poolSizes)
    {
        create(maxSetCount, poolSizes);
    }

    void DescriptorPool::create(const uint32_t maxSetCount, const std::vector<vk::DescriptorPoolSize>& poolSizes)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.setMaxSets(maxSetCount)
            .setPoolSizeCount(poolSizes.size())
            .setPPoolSizes(poolSizes.data());
        
        if(logicalDevice.createDescriptorPool(&poolInfo, nullptr, &pool) != vk::Result::eSuccess) throw std::runtime_error("Failed to create descriptor pool!\n");
    }

    DescriptorPool& DescriptorPool::addDescriptorSetLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        layouts.push_back(vk::DescriptorSetLayout());

        vk::DescriptorSetLayoutCreateInfo layoutInfo;
        layoutInfo.setBindingCount(bindings.size())
            .setPBindings(bindings.data());
        if(logicalDevice.createDescriptorSetLayout(&layoutInfo, nullptr, &layouts.back()) != vk::Result::eSuccess) throw std::runtime_error("Failed to create descriptor set layout!\n");

        return *this;
    }

    DescriptorPool& DescriptorPool::allocateDescriptorSets()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        sets.resize(layouts.size());
        vk::DescriptorSetAllocateInfo info;
        info.setDescriptorPool(pool)
            .setDescriptorSetCount(layouts.size())
            .setPSetLayouts(layouts.data());
        
        if(logicalDevice.allocateDescriptorSets(&info, sets.data()) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate descriptor sets!\n");

        return *this;
    }

    DescriptorPool& DescriptorPool::writeDescriptorSetImage(const uint32_t index, const uint32_t binding, const vk::DescriptorType& type, const vk::DescriptorImageInfo& imageInfo, const uint32_t descriptorIndex, const uint32_t descriptorCount)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        vk::WriteDescriptorSet write;
        write.setDstSet(sets[index])
            .setDstBinding(binding)
            .setDstArrayElement(descriptorIndex)
            .setDescriptorCount(descriptorCount)
            .setDescriptorType(type)
            .setPImageInfo(&imageInfo)
            .setPBufferInfo(nullptr)
            .setPTexelBufferView(nullptr);
        logicalDevice.updateDescriptorSets(1, &write, 0, nullptr);

        return *this;
    }

    DescriptorPool& DescriptorPool::writeDescriptorSetBuffer(const uint32_t index, const uint32_t binding, const vk::DescriptorType& type, const vk::DescriptorBufferInfo& bufferInfo, const uint32_t descriptorIndex = 0, const uint32_t descriptorCount = 1)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        vk::WriteDescriptorSet write;
        write.setDstSet(sets[index])
            .setDstBinding(binding)
            .setDstArrayElement(descriptorIndex)
            .setDescriptorCount(descriptorCount)
            .setDescriptorType(type)
            .setPImageInfo(nullptr)
            .setPBufferInfo(&bufferInfo)
            .setPTexelBufferView(nullptr);
        logicalDevice.updateDescriptorSets(1, &write, 0, nullptr);

        return *this;
    }

    DescriptorPool& DescriptorPool::copyDescriptorSet(const uint32_t srcSetIndex, 
        const uint32_t srcBinding,
        const uint32_t srcDescriptorIndex,
        const uint32_t dstSetIndex,
        const uint32_t dstBinding,
        const uint32_t dstDescrtiprorIndex,
        const uint32_t descriptorCount)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        vk::CopyDescriptorSet copy;
        copy.setSrcSet(sets[srcSetIndex])
            .setSrcBinding(srcBinding)
            .setSrcArrayElement(srcDescriptorIndex)
            .setDstSet(sets[dstSetIndex])
            .setDstBinding(dstBinding)
            .setDstArrayElement(dstDescrtiprorIndex)
            .setDescriptorCount(descriptorCount);
        logicalDevice.updateDescriptorSets(0, nullptr, 1, &copy);

        return *this;
    }

    vk::PipelineLayout DescriptorPool::getPipelineLayout(const std::vector<uint32_t>& descriptorSetLayoutIndices) const
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        std::vector<vk::DescriptorSetLayout> chosenLayouts(descriptorSetLayoutIndices.size());
        int i = 0;
        for(const auto id : descriptorSetLayoutIndices)
        {
            chosenLayouts[i] = layouts[id];
            ++i;
        }

        vk::PipelineLayout layout;

        vk::PipelineLayoutCreateInfo info;
        info.setSetLayoutCount(chosenLayouts.size())
            .setPSetLayouts(chosenLayouts.data())
            .setPushConstantRangeCount(0)
            .setPPushConstantRanges(nullptr);
        logicalDevice.createPipelineLayout(&info, nullptr, &layout);

        return layout;
    }

    void DescriptorPool::destroyPipelineLayout(vk::PipelineLayout& layout) const
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        logicalDevice.destroyPipelineLayout(layout, nullptr);
        layout = vk::PipelineLayout();
    }

    void DescriptorPool::destroy()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(layouts.size() != 0)
        {
            if(layouts[0])
            {
                for(auto& layout : layouts)
                {
                    logicalDevice.destroyDescriptorSetLayout(layout, nullptr);
                    layout = vk::DescriptorSetLayout();
                }
            }
        }
        if(sets.size() != 0)
        {
            if(sets[0])
            {
                logicalDevice.freeDescriptorSets(pool, sets.size(), sets.data());
                for(auto& set : sets)
                {
                    set = vk::DescriptorSet();
                }
            }
        }
        if(pool)
        {
            logicalDevice.destroyDescriptorPool(pool, nullptr);
        }
    }

    DescriptorPool::~DescriptorPool()
    {
        destroy();
    }
}