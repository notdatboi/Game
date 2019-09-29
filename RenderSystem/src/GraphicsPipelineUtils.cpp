#include<GraphicsPipelineUtils.hpp>

PipelineInfoBuilder::PipelineInfoBuilder(): info()
{
    vertexInputState = {};
    inputAssemblyState = {};
    tessellationState = {};
    viewportState = {};
    rasterizationState = {};
    multisampleState = {};
    depthStencilState = {};
    colorBlendState = {};
    dynamicState = {};
    info = 
    {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        nullptr,
        0
    };
}

void PipelineInfoBuilder::setShaderStages(const Array<ShaderStageInfo>& stages)
{
    shaderStages.create(stages.getSize());
    for(auto ind = 0; ind < stages.getSize(); ++ind)
    {
        shaderStages[ind] = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            stages[ind].stage,
            stages[ind].module,
            "main",
            nullptr
        };
    }
    info.stageCount = shaderStages.getSize();
    info.pStages = shaderStages.getPtr();
}

void PipelineInfoBuilder::setVertexInputState(const bool enable, const Array<VkVertexInputBindingDescription>& bindings, const Array<VkVertexInputAttributeDescription>& attributes)
{
    if(!enable)
    {
        info.pVertexInputState = nullptr;
    }
    else
    {
        vertexInputState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            nullptr,
            0,
            bindings.getSize(),
            bindings.getPtr(),
            attributes.getSize(),
            attributes.getPtr()
        };
        info.pVertexInputState = &vertexInputState;
    }
}

void PipelineInfoBuilder::setInputAssemblyState(const bool enable, const VkPrimitiveTopology topology, const VkBool32 enablePrimitiveRestart)
{
    if(!enable)
    {
        info.pInputAssemblyState = nullptr;
    }
    else
    {
        inputAssemblyState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr,
            0,
            topology,
            enablePrimitiveRestart
        };
        info.pInputAssemblyState = &inputAssemblyState;
    }
}

void PipelineInfoBuilder::setTessellationState(const bool enable, const uint32_t controlPoints)
{
    if(!enable)
    {
        info.pTessellationState = nullptr;
    }
    else
    {
        tessellationState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            nullptr,
            0,
            controlPoints
        };
        info.pTessellationState = &tessellationState;
    }
}

void PipelineInfoBuilder::setViewportState(const bool enable, const Array<VkViewport>& viewports, const Array<VkRect2D>& scissors)
{
    if(!enable)
    {
        info.pViewportState = nullptr;
    }
    else
    {
        viewportState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            nullptr,
            0,
            viewports.getSize(),
            viewports.getPtr(),
            scissors.getSize(),
            scissors.getPtr()
        };
        info.pViewportState = &viewportState;
    }
}

void PipelineInfoBuilder::setRasterizationState(const VkBool32 enableDepthClamp, const VkPolygonMode polygonMode, const VkCullModeFlags culling, const VkFrontFace frontFace, const VkBool32 enableDepthBias, const float constantFactor, const float clamp, const float slopeFactor)
{
    rasterizationState = 
    {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        nullptr,
        0,
        enableDepthClamp,
        VK_FALSE,
        polygonMode,
        culling,
        frontFace,
        enableDepthBias,
        constantFactor,
        clamp,
        slopeFactor,
        1
    };
    info.pRasterizationState = &rasterizationState;
}

void PipelineInfoBuilder::setMultisampleState(const bool enable, const VkSampleCountFlagBits samples, const VkBool32 enableSampleShading, const float minSampleShading, const VkSampleMask* sampleMask, const VkBool32 alphaToCoverage, const VkBool32 alphaToOne)
{
    if(!enable)
    {
        info.pMultisampleState = nullptr;
    }
    else
    {
        multisampleState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            nullptr,
            0,
            samples,
            enableSampleShading,
            minSampleShading,
            sampleMask,
            alphaToCoverage,
            alphaToOne
        };
        info.pMultisampleState = &multisampleState;
    }
}

void PipelineInfoBuilder::setDepthStencilState(const bool enable, const VkBool32 enableDepthTest, const VkBool32 enableDepthWrite, const VkCompareOp compareOp, const VkBool32 enableDepthBoundsTest, const VkBool32 enableStencilTest, const VkStencilOpState& front, const VkStencilOpState& back, const float minDepthBounds, const float maxDepthBounds)
{
    if(!enable)
    {
        info.pDepthStencilState = nullptr;
    }
    else
    {
        depthStencilState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            nullptr,
            0,
            enableDepthTest,
            enableDepthWrite,
            compareOp,
            enableDepthBoundsTest,
            enableStencilTest,
            front,
            back,
            minDepthBounds,
            maxDepthBounds
        };
        info.pDepthStencilState = &depthStencilState;
    }
}

void PipelineInfoBuilder::setColorBlendState(const bool enable, const VkBool32 enableLogicOp, const VkLogicOp logicOp, const Array<VkPipelineColorBlendAttachmentState>& attachments)
{
    if(!enable)
    {
        info.pColorBlendState = nullptr;
    }
    else
    {
        colorBlendState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            nullptr,
            0,
            enableLogicOp,
            logicOp,
            attachments.getSize(),
            attachments.getPtr(),
            {0, 0, 0, 0}
        };
        info.pColorBlendState = &colorBlendState;
    }
}

void PipelineInfoBuilder::setDynamicState(const bool enable, const Array<VkDynamicState>& dynamicStates)
{
    if(!enable)
    {
        info.pDynamicState = nullptr;
    }
    else
    {
        dynamicState = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            nullptr,
            0,
            dynamicStates.getSize(),
            dynamicStates.getPtr()
        };
        info.pDynamicState = &dynamicState;
    }
}

void PipelineInfoBuilder::setLayout(const VkPipelineLayout* layout)
{
    info.layout = *layout;
}

void PipelineInfoBuilder::setRenderPass(const VkRenderPass* renderPass, const uint32_t subpass)
{
    info.renderPass = *renderPass;
    info.subpass = subpass;
}

void PipelineInfoBuilder::setBasePipeline(const VkPipeline* basePipeline, const int32_t basePipelineIndex)
{
    info.basePipelineHandle = *basePipeline;
    info.basePipelineIndex = basePipelineIndex;
}

const VkGraphicsPipelineCreateInfo PipelineInfoBuilder::generatePipelineInfo() const
{
    return info;
}

PipelineInfoBuilder::~PipelineInfoBuilder()
{
}

PipelinePool::PipelinePool(){}

void PipelinePool::create(const System* system, const uint32_t pipelineCount)
{
    this->system = system;
    pipelines.create(pipelineCount);
}

void PipelinePool::createPipeline(const uint32_t index, const VkGraphicsPipelineCreateInfo& pipelineInfo, const VkPipelineCache& cache)
{
    checkResult(vkCreateGraphicsPipelines(system->getDevice(), cache, 1, &pipelineInfo, nullptr, &pipelines[index]), "Failed to create pipeline.\n");
}

const VkPipeline& PipelinePool::operator[](const uint32_t index) const
{
    return pipelines[index];
}

void PipelinePool::destroy()
{
    for(auto ind = 0; ind < pipelines.getSize(); ++ind)
    {
        if(pipelines[ind])
        {
            vkDestroyPipeline(system->getDevice(), pipelines[ind], nullptr);
            pipelines[ind] = 0;
        }
    }
    pipelines.clear();
}

PipelinePool::~PipelinePool()
{
    destroy();
}