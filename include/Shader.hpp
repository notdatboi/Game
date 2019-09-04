#ifndef SHADER_HPP
#define SHADER_HPP

#include<glslang/Public/ShaderLang.h>
#include<SPIRV/GlslangToSpv.h>
#include<Glslang/DirStackFileIncluder.h>
#include<System.hpp>

class Shader
{
public:
    Shader();
    void create(const System* system, const char* filename);
    const ShaderStageInfo& getShader() const;
    void destroy();
    ~Shader();
private:
    static const int clientInputSematicsVersion = 100;
    static const glslang::EshTargetClientVersion vulkanClientVersion = glslang::EShTargetVulkan_1_0;
    static const glslang::EShTargetLanguageVersion targetLangVersion = glslang::EShTargetSpv_1_0;
    const System* system;
    ShaderStageInfo shader;
};

#endif