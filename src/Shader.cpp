#include<Shader.hpp>
#include<iostream>
#include<string>

const std::string getFilePath(const std::string& filename)
{
    const auto folderNameEndIndex = filename.find_last_of("/\\");
    return filename.substr(0, folderNameEndIndex);
}

const std::string getSuffix(const std::string& filename)
{
    const auto dotPos = filename.rfind('.');
    return (dotPos != std::string::npos) ? filename.substr(dotPos + 1) : "";
}

const EShLanguage parseAndSetType(const std::string& suffix, VkShaderStageFlagBits& type)
{
    if(suffix == "vert")
    {
        type = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        return EShLangVertex;
    }
    else if(suffix == "tesc")
    {
        type = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        return EShLangTessControl;
    }
    else if(suffix == "tese")
    {
        type = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        return EShLangTessEvaluation;
    }
    else if(suffix == "geom")
    {
        type = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
        return EShLangGeometry;
    }
    else if(suffix == "frag")
    {
        type = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        return EShLangFragment;
    }
    else if(suffix == "comp")
    {
        type = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
        return EShLangCompute;
    }
    else
    {
        std::cout << "Invalid shader.\n";
    }
}

void Shader::create(const System* system, const char* filename)
{
    this->system = system;

    std::vector<uint32_t> compiled;

    glslang::InitializeProcess();
    
    std::ifstream file(filename);

    if(!file.is_open())
    {
        glslang::FinalizeProcess();
        throw std::runtime_error("Shader file not found.");
    }

    std::string glslInput{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    const char* glslInputCStr = glslInput.c_str();

    EShLanguage shaderType = parseAndSetType(std::move(getSuffix(filename)), shader.stage);
    glslang::TShader glslShader(shaderType);
    glslShader.setStrings(&glslInputCStr, 1);
    glslShader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, clientInputSematicsVersion);
    glslShader.setEnvClient(glslang::EShClientVulkan, vulkanClientVersion);
    glslShader.setEnvTarget(glslang::EshTargetSpv, targetLangVersion);

    const EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

    DirStackFileIncluder includer;
    includer.pushExternalLocalDirectory(getFilePath(filename));

    std::string glslPreprocessed;
    if(!glslShader.preprocess(&DefaultTBuiltInResource, clientInputSematicsVersion, ENoProfile, false, false, messages, &glslPreprocessed, includer))
    {
        std::cout << "GLSL Preprocessing Failed for: " << filename << std::endl;
        std::cout << glslShader.getInfoLog() << std::endl;
        std::cout << glslShader.getInfoDebugLog() << std::endl;
    }

    const char* preprocessedCStr = glslPreprocessed.c_str();
    glslShader.setStrings(&preprocessedCStr, 1);

    if (!glslShader.parse(&DefaultTBuiltInResource, clientInputSematicsVersion, false, messages))
    {
        std::cout << "GLSL Parsing Failed for: " << filename << std::endl;
        std::cout << glslShader.getInfoLog() << std::endl;
        std::cout << glslShader.getInfoDebugLog() << std::endl;
    }

    glslang::TProgram program;
    program.addShader(&glslShader);
    if(!program.link(messages))
    {
        std::cout << "GLSL Linking Failed for: " << filename << std::endl;
        std::cout << glslShader.getInfoLog() << std::endl;
        std::cout << glslShader.getInfoDebugLog() << std::endl;
    }

    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    glslang::GlslangToSpv(*program.getIntermediate(shaderType), compiled, &logger, &spvOptions);

    VkShaderModuleCreateInfo info;
    info.codeSize = compiled.size() * sizeof(uint32_t);
    info.pCode = compiled.data();

    checkResult(vkCreateShaderModule(system->getDevice(), &info, nullptr, &shader.module), "Failed to create shader.\n");
    glslang::FinalizeProcess();
}

const ShaderStageInfo& Shader::getShader() const
{
    return shader;
}

void Shader::destroy()
{
    if(shader.module)
    {
        vkDestroyShaderModule(system->getDevice(), shader.module, nullptr);
        shader.module = 0;
    }
}

Shader::~Shader()
{
    destroy();
}