#ifndef SPARK_SHADER_SET_HPP
#define SPARK_SHADER_SET_HPP

#include"SparkIncludeBase.hpp"
#include<vector>
#include<string>
#include<fstream>
#include"System.hpp"
#include<Shader.hpp>

namespace spk
{
    class ShaderSet
    {
    public:
        ShaderSet();
        ShaderSet(const std::vector<std::string>& shaderFilenames);
        void create(const std::vector<std::string>& shaderFilenames);
        const std::vector<vk::PipelineShaderStageCreateInfo>& getShaderStages() const;
        const uint32_t getIdentifier() const;
        ~ShaderSet();
    private:
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        std::vector<Shader> shaders;
        void destroy();
    };
}

#endif