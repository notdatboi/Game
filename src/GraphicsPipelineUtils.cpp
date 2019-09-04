#include<GraphicsPipelineUtils.hpp>

PipelineInfoBuilder::PipelineInfoBuilder()
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
    layout = nullptr;
    renderPass = nullptr;
    subpass = {};
    basePipeline = nullptr;
    basePipelineIndex = {};
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
}

void PipelineInfoBuilder::setVertexInputState(const Array<VkVertexInputBindingDescription>& bindings, const Array<VkVertexInputAttributeDescription>& attributes)
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
}

void PipelineInfoBuilder::setInputAssemblyState(const VkPrimitiveTopology topology, const VkBool32 enablePrimitiveRestart)
{
    inputAssemblyState = 
    {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        0,
        topology,
        enablePrimitiveRestart
    };
}

void PipelineInfoBuilder::setTessellationState(const uint32_t controlPoints)
{
    tessellationState = 
    {
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        nullptr,
        0,
        controlPoints
    };
}

void PipelineInfoBuilder::setViewportState(const Array<VkViewport>& viewports, const Array<VkRect2D>& scissors)
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
}

void PipelineInfoBuilder::setMultisampleState(const VkSampleCountFlagBits samples, const VkBool32 enableSampleShading, const float minSampleShading, const VkSampleMask* sampleMask, const VkBool32 alphaToCoverage, const VkBool32 alphaToOne)
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
}

void PipelineInfoBuilder::setDepthStencilState(const VkBool32 enableDepthTest, const VkBool32 enableDepthWrite, const VkCompareOp compareOp, const VkBool32 enableDepthBoundsTest, const VkBool32 enableStencilTest, const VkStencilOpState& front, const VkStencilOpState& back, const float minDepthBounds, const float maxDepthBounds)
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
}

void PipelineInfoBuilder::setColorBlendState(const VkBool32 enableLogicOp, const VkLogicOp logicOp, const Array<VkPipelineColorBlendAttachmentState>& attachments)
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
}

void PipelineInfoBuilder::setDynamicState(const Array<VkDynamicState>& dynamicStates)
{
    dynamicState = 
    {
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        nullptr,
        0,
        dynamicStates.getSize(),
        dynamicStates.getPtr()
    };
}

void PipelineInfoBuilder::setLayout(const VkPipelineLayout* layout)
{
    this->layout = layout;
}

void PipelineInfoBuilder::setRenderPass(const VkRenderPass* renderPass, const uint32_t subpass)
{
    this->renderPass = renderPass;
    this->subpass = subpass;
}

void PipelineInfoBuilder::setBasePipeline(const VkPipeline* basePipeline, const int32_t basePipelineIndex)
{
    this->basePipeline = basePipeline;
    this->basePipelineIndex = basePipelineIndex;
}

const VkGraphicsPipelineCreateInfo PipelineInfoBuilder::generatePipelineInfo() const
{
    VkGraphicsPipelineCreateInfo info = 
    {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        nullptr,
        0,
        shaderStages.getSize(),
        shaderStages.getPtr(),
        &vertexInputState,
        &inputAssemblyState,
        &tessellationState,
        &viewportState,
        &rasterizationState,
        &multisampleState,
        &depthStencilState,
        &colorBlendState,
        &dynamicState,
        *layout,
        *renderPass,
        subpass,
        basePipeline ? *basePipeline : VkPipeline(),
        basePipelineIndex
    };
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
    pipelines.clean();
}

PipelinePool::~PipelinePool()
{
    destroy();
}