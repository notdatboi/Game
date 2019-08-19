#ifndef SPARK_SHADER_SET_HPP
#define SPARK_SHADER_SET_HPP

#include"SparkIncludeBase.hpp"
#include<vector>
#include<string>
#include<fstream>
#include<System.hpp>
#include<Shader.hpp>
#include<Texture.hpp>
#include<Uniform.hpp>
#include<map>

namespace spk
{
    class ShaderSet
    {
    public:
        ShaderSet();
        ShaderSet(const std::vector<std::string>& shaderFilenames);
        void create(const std::vector<std::string>& shaderFilenames);
        ShaderSet& addUniform(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn);
        ShaderSet& addTexture(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn);
        ShaderSet& addUniformArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count);
        ShaderSet& addTextureArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count);
        ShaderSet& saveConfiguration();             // set gaps are always removed
        ShaderSet& bindTexture(const uint32_t set, const uint32_t binding, const Texture& texture);
        ShaderSet& bindTextureArrayElement(const uint32_t set, const uint32_t binding, const uint32_t elementIndex, const Texture& texture);
        ShaderSet& bindUniform(const uint32_t set, const uint32_t binding, const Uniform& uniform);
        ShaderSet& bindUniformArrayElement(const uint32_t set, const uint32_t binding, const uint32_t elementIndex, const Uniform& uniform);

        const std::vector<vk::PipelineShaderStageCreateInfo>& getShaderStages() const;
        ~ShaderSet();
    private:
        void setDescriptorInfo(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const vk::DescriptorType type, const uint32_t count = 1);
        void writeTextureDescriptor(const Texture& texture, const uint32_t set, const uint32_t binding, const uint32_t index = 0);
        void writeUniformDescriptor(const Uniform& uniform, const uint32_t set, const uint32_t binding, const uint32_t index = 0);

        std::map<vk::DescriptorType, uint32_t> poolSizes;
        std::map<uint32_t, std::map<uint32_t, vk::DescriptorSetLayoutBinding> > setLayoutInfos;
        vk::DescriptorPool descriptorPool;
        std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
        std::vector<vk::DescriptorSet> descriptorSets;
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        std::vector<Shader> shaders;
        void destroy();
    };
}

#endif