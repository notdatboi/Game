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
#include<memory>

namespace spk
{
    class ShaderSet
    {
    public:
        ShaderSet();
        ShaderSet(const std::vector<std::string>& shaderFilenames);
        void create(const std::vector<std::string>& shaderFilenames);
        void addUniform(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn);
        void addTexture(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn);
        void addUniformArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count);
        void addTextureArray(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const uint32_t count);
        void saveConfiguration();             // descriptor set gaps are always removed
        ShaderSet& operator=(const ShaderSet& other);                   // copies config from the other shader set, so you can bind stuff at once (in case of usage of one shader set for multiple meshes)
        void bindTexture(const uint32_t set, const uint32_t binding, const Texture& texture);
        void bindTextureArrayElement(const uint32_t set, const uint32_t binding, const uint32_t elementIndex, const Texture& texture);
        void bindUniform(const uint32_t set, const uint32_t binding, const Uniform& uniform);
        void bindUniformArrayElement(const uint32_t set, const uint32_t binding, const uint32_t elementIndex, const Uniform& uniform);

        const bool areShadersEqual(const ShaderSet& other) const;

        const std::vector<vk::PipelineShaderStageCreateInfo>& getShaderStages() const;
        ~ShaderSet();
    private:
        void setDescriptorInfo(const uint32_t set, const uint32_t binding, const vk::ShaderStageFlags usedIn, const vk::DescriptorType type, const uint32_t count = 1);
        void writeTextureDescriptor(const Texture& texture, const uint32_t set, const uint32_t binding, const uint32_t index = 0);
        void writeUniformDescriptor(const Uniform& uniform, const uint32_t set, const uint32_t binding, const uint32_t index = 0);
        void createPool();
        void createSets();

        std::shared_ptr<std::map<vk::DescriptorType, uint32_t>> poolSizes;
        std::shared_ptr<std::map<uint32_t, std::map<uint32_t, vk::DescriptorSetLayoutBinding>>> setLayoutInfos;
        vk::DescriptorPool descriptorPool;
        std::shared_ptr<std::vector<vk::DescriptorSetLayout>> descriptorSetLayouts;
        std::vector<vk::DescriptorSet> descriptorSets;
        std::shared_ptr<std::vector<vk::PipelineShaderStageCreateInfo>> shaderStages;
        std::shared_ptr<std::vector<Shader>> shaders;
        void destroy();
    };
}

#endif