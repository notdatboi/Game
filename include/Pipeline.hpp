#ifndef SPARK_PIPELINE_HPP
#define SPARK_PIPELINE_HPP

#include"SparkIncludeBase.hpp"
#include"System.hpp"
#include<vector>

namespace spk
{
    struct ShaderStages
    {
        std::vector<vk::PipelineShaderStageCreateInfo> stages;
    };

    struct VertexInputState
    {
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions;
        std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
    };

    struct InputAssemblyState
    {
        InputAssemblyState(): topology(vk::PrimitiveTopology::eTriangleList), enablePrimitiveRestart(false) {}
        vk::PrimitiveTopology topology;
        bool enablePrimitiveRestart;
    };

    //struct TesselationState{};

    struct ViewportState
    {
        vk::Viewport viewport;
        vk::Rect2D scissor;
    };

    struct RasterizationState
    {
        bool enableDepthClamp;
        vk::CullModeFlags cullMode;
        vk::FrontFace frontFace;
        // TODO: add depth bias to prevent depth fighting
    };

    struct MultisampleState
    {
        vk::SampleCountFlagBits rasterizationSampleCount;
        // bool sampleShadingEnabled and so on
    };

    struct DepthStencilState
    {
        bool enableDepthTest;
        bool writeTestResults;
        vk::CompareOp depthCompareOp;
        /*bool enableDepthBoundsTest;
        bool enableStencilTest;
        vk::StencilOpState front;
        vk::StencilOpState back;
        float minDepthBounds;
        float maxDepthBounds;*/ // TODO: enable stencil amd depth bound test features
    };

    struct ColorBlendState
    {
        std::vector<vk::PipelineColorBlendAttachmentState> attachmentStates;
    };

    struct DynamicState
    {
        std::vector<vk::DynamicState> states;
    };

    struct AdditionalInfo
    {
        vk::PipelineLayout layout;
        vk::RenderPass renderPass;
        uint32_t subpassIndex;
    };

    class Pipeline
    {
    public:
        Pipeline();
        Pipeline(const ShaderStages& shaderStages, 
            const VertexInputState& vertexInputState,
            const InputAssemblyState& inputAssemblyState,
            const ViewportState& viewportState,
            const RasterizationState& rasterizationState,
            const MultisampleState& multisampleState,
            const DepthStencilState& depthStencilState,
            const ColorBlendState& colorBlendState,
            const DynamicState& dynamicState,
            const AdditionalInfo& additionalInfo);
        void create(const ShaderStages& shaderStages, 
            const VertexInputState& vertexInputState,
            const InputAssemblyState& inputAssemblyState,
            const ViewportState& viewportState,
            const RasterizationState& rasterizationState,
            const MultisampleState& multisampleState,
            const DepthStencilState& depthStencilState,
            const ColorBlendState& colorBlendState,
            const DynamicState& dynamicState,
            const AdditionalInfo& additionalInfo);
        //vk::PipelineCache generateCache();
        const vk::Pipeline& getPipeline() const;
        const vk::PipelineLayout& getLayout() const;
        void destroy();
        ~Pipeline();
    private:
        vk::Pipeline pipeline;
        vk::PipelineLayout layout;
    };
}

#endif