#include<Shader.hpp>

namespace spk
{
    void Shader::load(const std::string& filename)
    {
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

        EShLanguage shaderType = parseAndSetType(getSuffix(filename));
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

        vk::ShaderModuleCreateInfo info;
        info.setCodeSize(compiled.size() * sizeof(uint32_t))
            .setPCode(compiled.data());

        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(logicalDevice.createShaderModule(&info, nullptr, &shader) != vk::Result::eSuccess)
        {
            glslang::FinalizeProcess();
            throw std::runtime_error("Failed to create shader module!\n");
        }
        glslang::FinalizeProcess();
    }

    const std::string Shader::getFilePath(const std::string& filename)
    {
        const auto folderNameEndIndex = filename.find_last_of("/\\");
        return filename.substr(0, folderNameEndIndex);
    }

    const std::string Shader::getSuffix(const std::string& filename)
    {
        const auto dotPos = filename.rfind('.');
        return (dotPos != std::string::npos) ? filename.substr(dotPos + 1) : "";
    }

    const EShLanguage Shader::parseAndSetType(const std::string& suffix)
    {
        if(suffix == "vert")
        {
            type = vk::ShaderStageFlagBits::eVertex;
            return EShLangVertex;
        }
        else if(suffix == "tesc")
        {
            type = vk::ShaderStageFlagBits::eTessellationControl;
            return EShLangTessControl;
        }
        else if(suffix == "tese")
        {
            type = vk::ShaderStageFlagBits::eTessellationEvaluation;
            return EShLangTessEvaluation;
        }
        else if(suffix == "geom")
        {
            type = vk::ShaderStageFlagBits::eGeometry;
            return EShLangGeometry;
        }
        else if(suffix == "frag")
        {
            type = vk::ShaderStageFlagBits::eFragment;
            return EShLangFragment;
        }
        else if(suffix == "comp")
        {
            type = vk::ShaderStageFlagBits::eCompute;
            return EShLangCompute;
        }
        else
        {
            throw std::invalid_argument("Invalid shader suffix given.\n");
        }
    }

    const vk::ShaderModule& Shader::getShader() const
    {
        return shader;
    }

    const vk::ShaderStageFlagBits Shader::getType() const
    {
        return type;
    }

    Shader::~Shader()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(shader)
        {
            logicalDevice.destroyShaderModule(shader, nullptr);
            shader = vk::ShaderModule();
        }
    }
}