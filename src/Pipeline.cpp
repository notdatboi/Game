#include"../include/Pipeline.hpp"

namespace spk
{
    Pipeline::Pipeline()
    {
        vertexInputInfo.setVertexBindingDescriptionCount(0)
            .setPVertexBindingDescriptions(nullptr)
            .setVertexAttributeDescriptionCount(0)
            .setPVertexAttributeDescriptions(nullptr);

        inputAssemblyInfo.setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(false);
        
        tessellationInfo.setPatchControlPoints(0);

        viewportInfo.setScissorCount(0)
            .setPScissors(nullptr)
            .setViewportCount(0)
            .setPViewports(nullptr);

        rasterizationInfo.setDepthClampEnable(false)
            .setRasterizerDiscardEnable(false)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eNone)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthBiasEnable(false)
            .setDepthBiasConstantFactor(0)
            .setDepthBiasClamp(0)
            .setDepthBiasSlopeFactor(0)
            .setLineWidth(1);

        multisampleInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setSampleShadingEnable(false)
            .setMinSampleShading(0)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(false)
            .setAlphaToOneEnable(false);

        depthStencilInfo.setDepthTestEnable(false)
            .setDepthWriteEnable(false)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(false)
            .setStencilTestEnable(false)
            .setFront({})
            .setBack({})
            .setMinDepthBounds(0)
            .setMaxDepthBounds(1);
        
        colorBlendInfo.setLogicOpEnable(false)
            .setLogicOp(vk::LogicOp::eEquivalent)
            .setAttachmentCount(0)
            .setPAttachments(nullptr)
            .setBlendConstants({1, 1, 1, 1});

        dynamicInfo.setDynamicStateCount(0)
            .setPDynamicStates(nullptr);
    }

    void Pipeline::setShaders(const ShaderSet& shaders)
    {
        shaderStageInfos = shaders.getShaderStages();
        layout = *shaders.getPipelineLayoutPtr().get();
    }

    void Pipeline::setVertexDescription(const VertexDescription& description)
    {
        auto* bindingPtrs = new vk::VertexInputBindingDescription[description.bindingDescriptions.size()];
        auto* attrPtrs = new vk::VertexInputAttributeDescription[description.attributeDescriptions.size()];
        memcpy(bindingPtrs, description.bindingDescriptions.data(), sizeof(vk::VertexInputBindingDescription) * description.bindingDescriptions.size());
        memcpy(attrPtrs, description.attributeDescriptions.data(), sizeof(vk::VertexInputAttributeDescription) * description.attributeDescriptions.size());
        vertexInputInfo.setVertexBindingDescriptionCount(description.bindingDescriptions.size())
            .setPVertexBindingDescriptions(bindingPtrs)
            .setVertexAttributeDescriptionCount(description.attributeDescriptions.size())
            .setPVertexAttributeDescriptions(attrPtrs);

    }

    void Pipeline::setInputAssemblyState(const vk::PrimitiveTopology topology, const bool primitiveRestartEnable)
    {
        inputAssemblyInfo.setTopology(topology)
            .setPrimitiveRestartEnable(primitiveRestartEnable);
    }

    void Pipeline::setTessellationState(const uint32_t patchControlPoints)
    {
        tessellationInfo.setPatchControlPoints(patchControlPoints);
    }

    void Pipeline::setViewportState(const std::vector<vk::Viewport> viewports, const std::vector<vk::Rect2D> scissors)
    {
        auto* viewportPtrs = new vk::Viewport[viewports.size()];
        auto* scissorPtrs = new vk::Rect2D[scissors.size()];
        memcpy(viewportPtrs, viewports.data(), sizeof(vk::Viewport) * viewports.size());
        memcpy(scissorPtrs, scissors.data(), sizeof(vk::Rect2D) * scissors.size());
        viewportInfo.setViewportCount(viewports.size())
            .setPViewports(viewportPtrs)
            .setScissorCount(scissors.size())
            .setPScissors(scissorPtrs);
    }

    void Pipeline::setRasterizationState(const bool depthClampEnable,
        const bool rasterizedDiscardEnable,
        const vk::PolygonMode polygonMode,
        const vk::CullModeFlags cullMode,
        const vk::FrontFace frontFace,
        const bool depthBiasEnable,
        const float depthBiasConstantFactor,
        const float depthBiasClamp,
        const float depthBiasSlopeFactor,
        const float lineWidth)
    {
        rasterizationInfo.setDepthClampEnable(depthClampEnable)
            .setRasterizerDiscardEnable(rasterizedDiscardEnable)
            .setPolygonMode(polygonMode)
            .setCullMode(cullMode)
            .setFrontFace(frontFace)
            .setDepthBiasEnable(depthBiasEnable)
            .setDepthBiasConstantFactor(depthBiasConstantFactor)
            .setDepthBiasClamp(depthBiasClamp)
            .setDepthBiasSlopeFactor(depthBiasSlopeFactor)
            .setLineWidth(lineWidth);
    }

    void Pipeline::setRasterizationState(const vk::PolygonMode polygonMode,
        const vk::CullModeFlags cullMode,
        const vk::FrontFace frontFace)
    {
        rasterizationInfo.setPolygonMode(polygonMode)
            .setCullMode(cullMode)
            .setFrontFace(frontFace);
    }

    void Pipeline::setMultisampleState(const vk::SampleCountFlagBits rasterizationSamples,
        const bool sampleShadingEnable,
        const float minSampleShading,
        const vk::SampleMask* sampleMask,
        const bool alphaToCoverageEnable,
        const bool alphaToOneEnable)
    {
        multisampleInfo.setRasterizationSamples(rasterizationSamples)
            .setSampleShadingEnable(sampleShadingEnable)
            .setMinSampleShading(minSampleShading)
            .setPSampleMask(sampleMask)
            .setAlphaToCoverageEnable(alphaToCoverageEnable)
            .setAlphaToOneEnable(alphaToOneEnable);
    }

    void Pipeline::setDepthStencilState(const bool depthTestEnable,
        const bool depthWriteEnable,
        const vk::CompareOp compareOp,
        const bool depthBoundsTestEnable,
        const bool stencilTestEnable,
        const vk::StencilOpState front,
        const vk::StencilOpState back,
        const float minDepthBounds,
        const float maxDepthBounds)
    {
        depthStencilInfo.setDepthTestEnable(depthTestEnable)
            .setDepthWriteEnable(depthWriteEnable)
            .setDepthCompareOp(compareOp)
            .setDepthBoundsTestEnable(depthBoundsTestEnable)
            .setStencilTestEnable(stencilTestEnable)
            .setFront(front)
            .setBack(back)
            .setMinDepthBounds(minDepthBounds)
            .setMaxDepthBounds(maxDepthBounds);
    }

    void Pipeline::setDepthStencilState(const bool enable, const vk::CompareOp compareOp)
    {
        depthStencilInfo.setDepthBoundsTestEnable(enable)
            .setDepthCompareOp(compareOp);
    }

    void Pipeline::setColorBlendState(const bool logicOpEnable,
        const vk::LogicOp logicOp,
        const std::vector<vk::PipelineColorBlendAttachmentState> attachments,
        const std::array<float, 4> blendConstants)
    {
        auto* attachmentPtrs = new vk::PipelineColorBlendAttachmentState[attachments.size()];
        memcpy(attachmentPtrs, attachments.data(), sizeof(vk::PipelineColorBlendAttachmentState) * attachments.size());
        colorBlendInfo.setLogicOpEnable(logicOpEnable)
            .setLogicOp(logicOp)
            .setAttachmentCount(attachments.size())
            .setPAttachments(attachmentPtrs)
            .setBlendConstants(blendConstants);
    }

    void Pipeline::setColorBlendState(const uint32_t renderTargetCount)
    {
        vk::PipelineColorBlendAttachmentState defaultState;
        defaultState.setBlendEnable(false)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        std::vector<vk::PipelineColorBlendAttachmentState> states(renderTargetCount, defaultState);
        auto* attachmentPtrs = new vk::PipelineColorBlendAttachmentState[renderTargetCount];
        memcpy(attachmentPtrs, states.data(), sizeof(vk::PipelineColorBlendAttachmentState) * states.size());

        colorBlendInfo.setAttachmentCount(renderTargetCount)
            .setPAttachments(attachmentPtrs);
    }

    void Pipeline::setDynamicState(const std::vector<vk::DynamicState> dynamicStates)
    {
        auto* dynamicPtrs = new vk::DynamicState[dynamicStates.size()];
        memcpy(dynamicPtrs, dynamicStates.data(), sizeof(vk::DynamicState) * dynamicStates.size());
        dynamicInfo.setDynamicStateCount(dynamicStates.size())
            .setPDynamicStates(dynamicPtrs);
    }

    void Pipeline::setRenderPass(const vk::RenderPass& renderPass)
    {
        this->renderPass = renderPass;
    }

    void Pipeline::setSubpassIndex(const uint32_t subpassIndex)
    {
        this->subpass = subpassIndex;
    }

    void Pipeline::setBaseHandleAndIndex(const vk::Pipeline& baseHandle, const int32_t baseIndex)
    {
        this->baseHandle = baseHandle;
        this->baseIndex = baseIndex;
    }

    void Pipeline::create()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        pipelineInfo.setStageCount(shaderStageInfos.size())
            .setPStages(shaderStageInfos.data())
            .setPVertexInputState(&vertexInputInfo)
            .setPInputAssemblyState(&inputAssemblyInfo)
            .setPTessellationState(&tessellationInfo)
            .setPViewportState(&viewportInfo)
            .setPRasterizationState(&rasterizationInfo)
            .setPMultisampleState(&multisampleInfo)
            .setPDepthStencilState(&depthStencilInfo)
            .setPColorBlendState(&colorBlendInfo)
            .setPDynamicState(&dynamicInfo)
            .setLayout(layout)
            .setRenderPass(renderPass)
            .setSubpass(subpass);
        if(baseHandle)
        {
            pipelineInfo.setBasePipelineHandle(baseHandle)
                .setBasePipelineIndex(baseIndex);
        }
        if(logicalDevice.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &pipeline) != vk::Result::eSuccess) throw std::runtime_error("Failed to create pipeline!\n");

        if(vertexInputInfo.pVertexAttributeDescriptions)
        {
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            delete[] vertexInputInfo.pVertexAttributeDescriptions;
        }
        if(vertexInputInfo.pVertexBindingDescriptions)
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            delete[] vertexInputInfo.pVertexBindingDescriptions;
        }
        if(viewportInfo.pViewports)
        {
            viewportInfo.viewportCount = 0;
            delete[] viewportInfo.pViewports;
        }
        if(viewportInfo.pScissors)
        {
            viewportInfo.scissorCount = 0;
            delete[] viewportInfo.pScissors;
        }
        if(colorBlendInfo.pAttachments)
        {
            colorBlendInfo.attachmentCount = 0;
            delete[] colorBlendInfo.pAttachments;
        }
        if(dynamicInfo.pDynamicStates)
        {
            dynamicInfo.dynamicStateCount = 0;
            delete[] dynamicInfo.pDynamicStates;
        }
    }

    const vk::Pipeline& Pipeline::getPipeline() const
    {
        return pipeline;
    }
    
    const vk::PipelineLayout& Pipeline::getLayout() const
    {
        return layout;
    }

    void Pipeline::destroy()
    {
        if(pipeline)
        {
            const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
            logicalDevice.destroyPipeline(pipeline, nullptr);
        }
        if(vertexInputInfo.pVertexAttributeDescriptions)
        {
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            delete[] vertexInputInfo.pVertexAttributeDescriptions;
        }
        if(vertexInputInfo.pVertexBindingDescriptions)
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            delete[] vertexInputInfo.pVertexBindingDescriptions;
        }
        if(viewportInfo.pViewports)
        {
            viewportInfo.viewportCount = 0;
            delete[] viewportInfo.pViewports;
        }
        if(viewportInfo.pScissors)
        {
            viewportInfo.scissorCount = 0;
            delete[] viewportInfo.pScissors;
        }
        if(colorBlendInfo.pAttachments)
        {
            colorBlendInfo.attachmentCount = 0;
            delete[] colorBlendInfo.pAttachments;
        }
        if(dynamicInfo.pDynamicStates)
        {
            dynamicInfo.dynamicStateCount = 0;
            delete[] dynamicInfo.pDynamicStates;
        }
    }

    Pipeline::~Pipeline()
    {
        destroy();
    }
}