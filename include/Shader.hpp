#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include"System.hpp"
#include<string>
#include<glslang/Public/ShaderLang.h>
#include<SPIRV/GlslangToSpv.h>
#include<Glslang/DirStackFileIncluder.h>
#include<fstream>
#include<iostream>

namespace spk
{
    class Shader
    {
    public:
        void load(const std::string& filename);
        const vk::ShaderModule& getShader() const;
        const vk::ShaderStageFlagBits getType() const;
        const bool operator==(const Shader& other) const;
        const bool operator!=(const Shader& other) const;
        ~Shader();
    private:
        const std::string getFilePath(const std::string& filename);
        const std::string getSuffix(const std::string& filename);
        const EShLanguage parseAndSetType(const std::string& suffix);
        static const int clientInputSematicsVersion = 100;
        static const glslang::EshTargetClientVersion vulkanClientVersion = glslang::EShTargetVulkan_1_0;
        static const glslang::EShTargetLanguageVersion targetLangVersion = glslang::EShTargetSpv_1_0;
        vk::ShaderModule shader;
        vk::ShaderStageFlagBits type;
    };
}
#endif