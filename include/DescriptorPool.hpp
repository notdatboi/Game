#ifndef SPARK_DESCRIPTOR_POOL_HPP
#define SPARK_DESCRIPTOR_POOL_HPP

#include"SparkIncludeBase.hpp"
#include"System.hpp"

namespace spk
{
    class DescriptorPool
    {
    public:
        DescriptorPool();
        DescriptorPool(const uint32_t maxSetCount, const std::vector<vk::DescriptorPoolSize>& poolSizes, const vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlags());
        void create(const uint32_t maxSetCount, const std::vector<vk::DescriptorPoolSize>& poolSizes, const vk::DescriptorPoolCreateFlags flags = vk::DescriptorPoolCreateFlags());
        DescriptorPool& addDescriptorSetLayout(const std::vector<vk::DescriptorSetLayoutBinding>& bindings);
        DescriptorPool& allocateDescriptorSets(const std::vector<uint32_t>& layoutIndices);
        DescriptorPool& writeDescriptorSetImage(const uint32_t index, const uint32_t binding, const vk::DescriptorType type, const vk::DescriptorImageInfo& imageInfo, const uint32_t descriptorIndex = 0, const uint32_t descriptorCount = 1);
        DescriptorPool& writeDescriptorSetBuffer(const uint32_t index, const uint32_t binding, const vk::DescriptorType type, const vk::DescriptorBufferInfo& bufferInfo, const uint32_t descriptorIndex = 0, const uint32_t descriptorCount = 1);
        DescriptorPool& copyDescriptorSet(const uint32_t srcSetIndex, 
            const uint32_t srcBinding,
            const uint32_t srcDescriptorIndex,
            const uint32_t dstSetIndex,
            const uint32_t dstBinding,
            const uint32_t dstDescrtiprorIndex,
            const uint32_t descriptorCount);
        vk::PipelineLayout getPipelineLayout(const std::vector<uint32_t>& descriptorSetLayoutIndices) const;
        const std::vector<vk::DescriptorSet> getDescriptorSets(const std::vector<uint32_t>& setIndices) const;
        void destroyPipelineLayout(vk::PipelineLayout& layout) const;

        void destroy();
        ~DescriptorPool();
    private:
        vk::DescriptorPool pool;
        std::vector<vk::DescriptorSetLayout> layouts;
        std::vector<vk::DescriptorSet> sets;
    };
}

#endif