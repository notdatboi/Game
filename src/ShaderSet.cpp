#include"../include/ShaderSet.hpp"

namespace spk
{
    ShaderSet::ShaderSet()
    {
    }

    ShaderSet::ShaderSet(const ShaderSet& set)
    {
        create(set.infos);
    }

    ShaderSet::ShaderSet(const std::vector<ShaderInfo>& shaders)
    {
        create(shaders);
    }

    ShaderSet& ShaderSet::operator=(const ShaderSet& set)
    {
        destroy();
        create(set.infos);
        return *this;
    }

    void ShaderSet::create(const std::vector<ShaderInfo>& shaders)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        infos = shaders;
        shaderModules.resize(shaders.size());
        shaderStages.resize(shaders.size());
        for(int i = 0; i < shaders.size(); ++i)
        {
            vk::ShaderModuleCreateInfo info;
            std::vector<char> code = getCode(shaders[i].filename);
            info.setCodeSize(code.size());
            info.setPCode(reinterpret_cast<const uint32_t*>(code.data()));

            logicalDevice.createShaderModule(&info, nullptr, &shaderModules[i].first);

            shaderModules[i].second = shaders[i].type;
        }
        for(int i = 0; i < shaderModules.size(); ++i)
        {
            shaderStages[i].setModule(shaderModules[i].first);
            shaderStages[i].setStage(shaderModules[i].second);
            shaderStages[i].setPName("main");
            shaderStages[i].setPSpecializationInfo(nullptr);
        }
    }

    const std::vector<vk::PipelineShaderStageCreateInfo>& ShaderSet::getShaderStages() const
    {
        return shaderStages;
    }

    std::vector<char> ShaderSet::getCode(const std::string& filename) const
    {
        std::ifstream fin;
        fin.open(filename, std::ios::ate | std::ios::binary);
        size_t size = fin.tellg();
        std::vector<char> code(size);
        fin.seekg(0);
        fin.read(code.data(), size);
        fin.close();
        return code;
    }

    void ShaderSet::destroy()
    {
        if(shaderModules.size() != 0 && shaderModules[0].first)
        {
            for(auto& module : shaderModules)
            {
                const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
                logicalDevice.destroyShaderModule(module.first, nullptr);
            }
        }
    }

    ShaderSet::~ShaderSet()
    {
        destroy();
    }
}