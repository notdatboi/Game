#include"../include/Pipeline.hpp"

namespace spk
{
    Pipeline::Pipeline(){}

    Pipeline::Pipeline(const ShaderStages& shaderStages, 
        const VertexInputState& vertexInputState,
        const InputAssemblyState& inputAssemblyState,
        const ViewportState& viewportState,
        const RasterizationState& rasterizationState,
        const DepthStencilState& depthStencilState,
        const ColorBlendState& colorBlendState,
        const DynamicState& dynamicState,
        const AdditionalInfo& additionalInfo)
    {
        create(shaderStages, vertexInputState, inputAssemblyState, viewportState, rasterizationState, depthStencilState, colorBlendState, dynamicState, additionalInfo);
    }

    void Pipeline::create(const ShaderStages& shaderStages, 
        const VertexInputState& vertexInputState,
        const InputAssemblyState& inputAssemblyState,
        const ViewportState& viewportState,
        const RasterizationState& rasterizationState,
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
            .setPTessellationState(nullptr)
            .setPViewportState(&viewportInfo)
            .setPRasterizationState(&rasterizationInfo)
            .setPMultisampleState(nullptr)
            .setPDepthStencilState(&depthStencilInfo)
            .setPColorBlendState(&colorBlendInfo)
            .setPDynamicState(&dynamicInfo)
            .setLayout(layout)
            .setRenderPass(additionalInfo.renderPass)
            .setSubpass(additionalInfo.subpassIndex);
        
        if(logicalDevice.createGraphicsPipelines(vk::PipelineCache(), 1, &info, nullptr, &pipeline) != vk::Result::eSuccess) throw std::runtime_error("Failed to create pipeline!\n");
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
    }

    Pipeline::~Pipeline()
    {
        destroy();
    }
}