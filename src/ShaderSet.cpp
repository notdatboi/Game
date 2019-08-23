#include"../include/ShaderSet.hpp"

namespace spk
{
    ShaderSet::ShaderSet(): shaders()
    {
    }

    ShaderSet::ShaderSet(const std::vector<std::string>& shaderFilenames)
    {
        create(shaderFilenames);
    }

    void ShaderSet::create(const std::vector<std::string>& shaderFilenames)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        shaders.get()->resize(shaderFilenames.size());
        shaderStages.get()->resize(shaderFilenames.size());

        for(int i = 0; i < shaderFilenames.size(); ++i)
        {
            (*shaders.get())[i].load(shaderFilenames[i]);
        }
        for(int i = 0; i < shaderFilenames.size(); ++i)
        {
            (*shaderStages.get())[i].setModule((*shaders.get())[i].getShader());
            (*shaderStages.get())[i].setStage((*shaders.get())[i].getType());
            (*shaderStages.get())[i].setPName("main");
            (*shaderStages.get())[i].setPSpecializationInfo(nullptr);
        }
    }

    void ShaderSet::setDescriptorInfo(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const vk::DescriptorType type, const uint32_t count = 1)
    {
        (*poolSizes.get())[type]++;
        vk::DescriptorSetLayoutBinding setLayoutBinding;
        setLayoutBinding.setBinding(binding)
            .setDescriptorType(type)
            .setDescriptorCount(count)
            .setStageFlags(usedIn)
            .setPImmutableSamplers(nullptr);
        (*setLayoutInfos.get())[set][binding] = setLayoutBinding;
    }

    void ShaderSet::addUniform(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eUniformBuffer);
    }

    void ShaderSet::addTexture(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eCombinedImageSampler);
    }

    void ShaderSet::addUniformArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eUniformBuffer, count);
    }

    void ShaderSet::addTextureArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count)
    {
        setDescriptorInfo(set, binding, usedIn, vk::DescriptorType::eCombinedImageSampler, count);
    }

    void ShaderSet::createPool()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        uint32_t maxSets = 0;
        std::vector<vk::DescriptorPoolSize> assembledPoolSizes;
        for(auto poolSize : (*poolSizes.get()))
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
    }

    void ShaderSet::saveConfiguration()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        createPool();

        descriptorSetLayouts.get()->resize(setLayoutInfos.get()->size());
        descriptorSets.resize(descriptorSetLayouts.get()->size());

        uint32_t index = 0;
        for(auto& setLayoutInfo : (*setLayoutInfos.get()))
        {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            for(auto& bnd : setLayoutInfo.second)
            {
                bindings.push_back(bnd.second);
            }
            vk::DescriptorSetLayoutCreateInfo info;
            info.setBindingCount(bindings.size())
                .setPBindings(bindings.data());
            if(logicalDevice.createDescriptorSetLayout(&info, nullptr, &(*descriptorSetLayouts.get())[index]) != vk::Result::eSuccess) throw std::runtime_error("Failed to create descriptor set layout!\n");
            ++index;
        }

        createSets();
        createPipelineLayout();
    }

    void ShaderSet::createSets()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        vk::DescriptorSetAllocateInfo setAllocateInfo;
        setAllocateInfo.setDescriptorPool(descriptorPool)
            .setDescriptorSetCount(descriptorSetLayouts.get()->size())
            .setPSetLayouts(descriptorSetLayouts.get()->data());
        if(logicalDevice.allocateDescriptorSets(&setAllocateInfo, descriptorSets.data()) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate descriptor sets!\n");
    }

    void ShaderSet::createPipelineLayout()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.setSetLayoutCount(descriptorSetLayouts.get()->size())
            .setPSetLayouts(descriptorSetLayouts.get()->data())
            .setPushConstantRangeCount(0)
            .setPPushConstantRanges(nullptr);
        
        logicalDevice.createPipelineLayout(&pipelineLayoutInfo, nullptr, pipelineLayout.get());
    }

    ShaderSet& ShaderSet::operator=(const ShaderSet& other)
    {
        poolSizes = other.poolSizes;
        setLayoutInfos = other.setLayoutInfos;
        descriptorSetLayouts = other.descriptorSetLayouts;
        shaderStages = other.shaderStages;
        shaders = other.shaders;
        pipelineLayout = other.pipelineLayout;

        createPool();
        createSets();

        return *this;
    }

    void ShaderSet::writeTextureDescriptor(const Texture& texture, const uint32_t set, const uint32_t binding, const uint32_t index = 0)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        texture.waitUntilReady();

        vk::DescriptorImageInfo info;
        info.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setImageView(texture.getView())
            .setSampler(texture.getSampler());

        vk::WriteDescriptorSet write;
        write.setDstSet(descriptorSets[set])
            .setDstBinding(binding)
            .setDstArrayElement(index)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
            .setPImageInfo(&info)
            .setPBufferInfo(nullptr)
            .setPTexelBufferView(nullptr);

        logicalDevice.updateDescriptorSets(1, &write, 0, nullptr);
    }

    void ShaderSet::bindTexture(const uint32_t set, const uint32_t binding, const Texture& texture)
    {
        writeTextureDescriptor(texture, set, binding);
    }

    void ShaderSet::bindTextureArrayElement(const uint32_t set, const uint32_t binding, const uint32_t elementIndex, const Texture& texture)
    {
        writeTextureDescriptor(texture, set, binding, elementIndex);
    }

    void ShaderSet::writeUniformDescriptor(const Uniform& uniform, const uint32_t set, const uint32_t binding, const uint32_t index)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        uniform.waitUntilReady();

        vk::DescriptorBufferInfo info;
        info.setBuffer(uniform.getVkBuffer())
            .setOffset(0)
            .setRange(uniform.getSize());

        vk::WriteDescriptorSet write;
        write.setDstSet(descriptorSets[set])
            .setDstBinding(binding)
            .setDstArrayElement(index)
            .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
            .setPImageInfo(nullptr)
            .setPBufferInfo(&info)
            .setPTexelBufferView(nullptr);

        logicalDevice.updateDescriptorSets(1, &write, 0, nullptr);
    }

    void ShaderSet::bindUniform(const uint32_t set, const uint32_t binding, const Uniform& uniform)
    {
        writeUniformDescriptor(uniform, set, binding);
    }

    void ShaderSet::bindUniformArrayElement(const uint32_t set, const uint32_t binding, const uint32_t elementIndex, const Uniform& uniform)
    {
        writeUniformDescriptor(uniform, set, binding, elementIndex);
    }

    const bool ShaderSet::areShadersEqual(const ShaderSet& other) const
    {
        return shaders == other.shaders;
    }

    const std::vector<vk::PipelineShaderStageCreateInfo>& ShaderSet::getShaderStages() const
    {
        return *shaderStages.get();
    }

    void ShaderSet::destroy()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(descriptorPool)
        {
            logicalDevice.destroyDescriptorPool(descriptorPool, nullptr);
        }
        if(descriptorSetLayouts.use_count() == 1)
        {
            for(auto& layout : (*descriptorSetLayouts.get()))
            {
                if(layout)
                {
                    logicalDevice.destroyDescriptorSetLayout(layout, nullptr);
                    layout = vk::DescriptorSetLayout();
                }
            }
        }
        if(pipelineLayout.use_count() == 1)
        {
            if(pipelineLayout.get())
            {
                logicalDevice.destroyPipelineLayout(*pipelineLayout.get(), nullptr);
                pipelineLayout = nullptr;
            }
        }
    }

    ShaderSet::~ShaderSet()
    {
        destroy();
    }
}