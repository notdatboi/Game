#include"../include/ShaderSet.hpp"

namespace spk
{
    ShaderSet::ShaderSet()
    {
    }

    ShaderSet::ShaderSet(const std::vector<std::string>& shaderFilenames)
    {
        create(shaderFilenames);
    }

    void ShaderSet::create(const std::vector<std::string>& shaderFilenames)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        shaders.resize(shaderFilenames.size());
        shaderStages.resize(shaders.size());

        for(int i = 0; i < shaders.size(); ++i)
        {
            shaders[i].load(shaderFilenames[i]);
        }
        for(int i = 0; i < shaders.size(); ++i)
        {
            shaderStages[i].setModule(shaders[i].getShader());
            shaderStages[i].setStage(shaders[i].getType());
            shaderStages[i].setPName("main");
            shaderStages[i].setPSpecializationInfo(nullptr);
        }
    }

    void ShaderSet::setDescriptorInfo(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const vk::DescriptorType type, const uint32_t count = 1)
    {
        poolSizes[type]++;
        vk::DescriptorSetLayoutBinding setLayoutBinding;
        setLayoutBinding.setBinding(binding)
            .setDescriptorType(type)
            .setDescriptorCount(count)
            .setStageFlags(usedIn)
            .setPImmutableSamplers(nullptr);
        setLayoutInfos[set][binding] = setLayoutBinding;
    }

    ShaderSet& ShaderSet::addUniform(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eUniformBuffer);
    }

    ShaderSet& ShaderSet::addTexture(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eCombinedImageSampler);
    }

    ShaderSet& ShaderSet::addUniformArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eUniformBuffer, count);
    }

    ShaderSet& ShaderSet::addTextureArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eCombinedImageSampler, count);
    }
    
    ShaderSet& ShaderSet::saveConfiguration()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        uint32_t maxSets = 0;
        std::vector<vk::DescriptorPoolSize> assembledPoolSizes;
        for(auto poolSize : poolSizes)
        {
            assembledPoolSizes.push_back(vk::DescriptorPoolSize(poolSize.first, poolSize.second));
            maxSets += poolSize.second;
        }
        vk::DescriptorPoolCreateInfo descriptorPoolInfo;
        descriptorPoolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(maxSets)
            .setPoolSizeCount(assembledPoolSizes.size())
            .setPPoolSizes(assembledPoolSizes.data());
        if(logicalDevice.createDescriptorPool(&descriptorPoolInfo, nullptr, &descriptorPool) != vk::Result::eSuccess) throw std::runtime_error("Failed to create descriptor pool!\n");

        descriptorSetLayouts.resize(setLayoutInfos.size());
        descriptorSets.resize(descriptorSetLayouts.size());

        uint32_t index = 0;
        for(auto& setLayoutInfo : setLayoutInfos)
        {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            for(auto& bnd : setLayoutInfo.second)
            {
                bindings.push_back(bnd.second);
            }
            vk::DescriptorSetLayoutCreateInfo info;
            info.setBindingCount(bindings.size())
                .setPBindings(bindings.data());
            if(logicalDevice.createDescriptorSetLayout(&info, nullptr, &descriptorSetLayouts[index]) != vk::Result::eSuccess) throw std::runtime_error("Failed to create descriptor set layout!\n");
            ++index;
        }

        vk::DescriptorSetAllocateInfo setAllocateInfo;
        setAllocateInfo.setDescriptorPool(descriptorPool)
            .setDescriptorSetCount(descriptorSetLayouts.size())
            .setPSetLayouts(descriptorSetLayouts.data());
        if(logicalDevice.allocateDescriptorSets(&setAllocateInfo, descriptorSets.data()) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate descriptor sets!\n");
    }

    const std::vector<vk::PipelineShaderStageCreateInfo>& ShaderSet::getShaderStages() const
    {
        return shaderStages;
    }

    void ShaderSet::destroy()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(descriptorPool)
        {
            logicalDevice.destroyDescriptorPool(descriptorPool, nullptr);
        }
    }

    ShaderSet::~ShaderSet()
    {
        destroy();
    }
}