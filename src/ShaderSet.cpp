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

    const std::vector<vk::PipelineShaderStageCreateInfo>& ShaderSet::getShaderStages() const
    {
        return shaderStages;
    }

    void ShaderSet::destroy()
    {
    }

    ShaderSet::~ShaderSet()
    {
        destroy();
    }
}