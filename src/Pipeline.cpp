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

    Pipeline& Pipeline::addShaderStages(const std::vector<vk::PipelineShaderStageCreateInfo> stages)
    {
        shaderStageInfos = stages;

        return *this;
    }

    Pipeline& Pipeline::addVertexInputState(const std::vector<vk::VertexInputBindingDescription> bindingDescriptions, const std::vector<vk::VertexInputAttributeDescription> attributeDescriptions)
    {
        auto* bindingPtrs = new vk::VertexInputBindingDescription[bindingDescriptions.size()];
        auto* attrPtrs = new vk::VertexInputAttributeDescription[attributeDescriptions.size()];
        memcpy(bindingPtrs, bindingDescriptions.data(), sizeof(vk::VertexInputBindingDescription) * bindingDescriptions.size());
        memcpy(attrPtrs, attributeDescriptions.data(), sizeof(vk::VertexInputAttributeDescription) * attributeDescriptions.size());
        vertexInputInfo.setVertexBindingDescriptionCount(bindingDescriptions.size())
            .setPVertexBindingDescriptions(bindingPtrs)
            .setVertexAttributeDescriptionCount(attributeDescriptions.size())
            .setPVertexAttributeDescriptions(attrPtrs);

        return *this;
    }

    Pipeline& Pipeline::addInputAssemblyState(const vk::PrimitiveTopology topology, const bool primitiveRestartEnable)
    {
        inputAssemblyInfo.setTopology(topology)
            .setPrimitiveRestartEnable(primitiveRestartEnable);

        return *this;
    }

    Pipeline& Pipeline::addTessellationState(const uint32_t patchControlPoints)
    {
        tessellationInfo.setPatchControlPoints(patchControlPoints);

        return *this;
    }

    Pipeline& Pipeline::addViewportState(const std::vector<vk::Viewport> viewports, const std::vector<vk::Rect2D> scissors)
    {
        auto* viewportPtrs = new vk::Viewport[viewports.size()];
        auto* scissorPtrs = new vk::Rect2D[scissors.size()];
        memcpy(viewportPtrs, viewports.data(), sizeof(vk::Viewport) * viewports.size());
        memcpy(scissorPtrs, scissors.data(), sizeof(vk::Rect2D) * scissors.size());
        viewportInfo.setViewportCount(viewports.size())
            .setPViewports(viewportPtrs)
            .setScissorCount(scissors.size())
            .setPScissors(scissorPtrs);

        return *this;
    }

    Pipeline& Pipeline::addRasterizationState(const bool depthClampEnable,
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

        return *this;
    }

    Pipeline& Pipeline::addMultisampleState(const vk::SampleCountFlagBits rasterizationSamples,
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

        return *this;
    }

    Pipeline& Pipeline::addDepthStencilState(const bool depthTestEnable,
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

        return *this;
    }

    Pipeline& Pipeline::addColorBlendState(const bool logicOpEnable,
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

        return *this;
    }

    Pipeline& Pipeline::addDynamicState(const std::vector<vk::DynamicState> dynamicStates)
    {
        auto* dynamicPtrs = new vk::DynamicState[dynamicStates.size()];
        memcpy(dynamicPtrs, dynamicStates.data(), sizeof(vk::DynamicState) * dynamicStates.size());
        dynamicInfo.setDynamicStateCount(dynamicStates.size())
            .setPDynamicStates(dynamicPtrs);

        return *this;
    }

    Pipeline& Pipeline::setLayout(const vk::PipelineLayout& layout)
    {
        this->layout = layout;

        return *this;
    }

    Pipeline& Pipeline::setRenderPass(const vk::RenderPass& renderPass)
    {
        this->renderPass = renderPass;

        return *this;
    }

    Pipeline& Pipeline::setSubpassIndex(const uint32_t subpassIndex)
    {
        this->subpass = subpassIndex;

        return *this;
    }

    Pipeline& Pipeline::setBaseHandleAndIndex(const vk::Pipeline& baseHandle, const int32_t baseIndex)
    {
        this->baseHandle = baseHandle;
        this->baseIndex = baseIndex;

        return *this;
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

    /*void Pipeline::create(const ShaderStages& shaderStages, 
        const VertexInputState& vertexInputState,
        const InputAssemblyState& inputAssemblyState,
        const TessellationState& tessellationState,
        const ViewportState& viewportState,
        const RasterizationState& rasterizationState,
        const MultisampleState& multisampleState,
        const DepthStencilState& depthStencilState,
        const ColorBlendState& colorBlendState,
        const DynamicState& dynamicState,
        const AdditionalInfo& additionalInfo)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();

        layout = additionalInfo.layout;

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vertexInputInfo.setVertexBindingDescriptionCount(vertexInputState.bindingDescriptions.size())
            .setPVertexBindingDescriptions(vertexInputState.bindingDescriptions.data())
            .setVertexAttributeDescriptionCount(vertexInputState.attributeDescriptions.size())
            .setPVertexAttributeDescriptions(vertexInputState.attributeDescriptions.data());
        
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        inputAssemblyInfo.setTopology(inputAssemblyState.topology)
            .setPrimitiveRestartEnable(inputAssemblyState.enablePrimitiveRestart);

        vk::PipelineTessellationStateCreateInfo tessellationInfo;
        tessellationInfo.setPatchControlPoints(tessellationState.patchControlPointCount);

        vk::PipelineViewportStateCreateInfo viewportInfo;
        viewportInfo.setViewportCount(1)
            .setPViewports(&viewportState.viewport)
            .setScissorCount(1)
            .setPScissors(&viewportState.scissor);

        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        rasterizationInfo.setDepthClampEnable(rasterizationState.enableDepthClamp)
            .setRasterizerDiscardEnable(false)
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(rasterizationState.cullMode)
            .setFrontFace(rasterizationState.frontFace)
            .setDepthBiasEnable(false)
            .setLineWidth(1.0f);

        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        multisampleInfo.setRasterizationSamples(multisampleState.rasterizationSampleCount)
            .setSampleShadingEnable(false);

        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
        depthStencilInfo.setDepthTestEnable(depthStencilState.enableDepthTest)
            .setDepthWriteEnable(depthStencilState.writeTestResults)
            .setDepthCompareOp(depthStencilState.depthCompareOp)
            .setDepthBoundsTestEnable(false)
            .setStencilTestEnable(false);

        vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
        colorBlendInfo.setLogicOpEnable(false)
            .setAttachmentCount(colorBlendState.attachmentStates.size())
            .setPAttachments(colorBlendState.attachmentStates.data());

        vk::PipelineDynamicStateCreateInfo dynamicInfo;
        dynamicInfo.setDynamicStateCount(dynamicState.states.size())
            .setPDynamicStates(dynamicState.states.data());

        vk::GraphicsPipelineCreateInfo info;
        info.setStageCount(shaderStages.stages.size())
            .setPStages(shaderStages.stages.data())
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
            .setRenderPass(additionalInfo.renderPass)
            .setSubpass(additionalInfo.subpassIndex);
        
        if(logicalDevice.createGraphicsPipelines(vk::PipelineCache(), 1, &info, nullptr, &pipeline) != vk::Result::eSuccess) throw std::runtime_error("Failed to create pipeline!\n");
    }*/

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