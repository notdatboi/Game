#ifndef SPARK_PIPELINE_HPP
#define SPARK_PIPELINE_HPP

#include"SparkIncludeBase.hpp"
#include"System.hpp"
#include<vector>

namespace spk
{
    class Pipeline
    {
    public:
        Pipeline();
        Pipeline& addShaderStages(const std::vector<vk::PipelineShaderStageCreateInfo> stages);
        Pipeline& addVertexInputState(const std::vector<vk::VertexInputBindingDescription> bindingDescriptions, const std::vector<vk::VertexInputAttributeDescription> attributeDescriptions);
        Pipeline& addInputAssemblyState(const vk::PrimitiveTopology topology, const bool primitiveRestartEnable);
        Pipeline& addTessellationState(const uint32_t patchControlPoints);
        Pipeline& addViewportState(const std::vector<vk::Viewport> viewports, const std::vector<vk::Rect2D> scissors);
        Pipeline& addRasterizationState(const bool depthClampEnable,
            const bool rasterizedDiscardEnable,
            const vk::PolygonMode polygonMode,
            const vk::CullModeFlags cullMode,
            const vk::FrontFace frontFace,
            const bool depthBiasEnable,
            const float depthBiasConstantFactor,
            const float depthBiasClamp,
            const float depthBiasSlopeFactor,
            const float lineWidth);
        Pipeline& addMultisampleState(const vk::SampleCountFlagBits rasterizationSamples,
            const bool sampleShadingEnable,
            const float minSampleShading,
            const vk::SampleMask* sampleMask,
            const bool alphaToCoverageEnable,
            const bool alphaToOneEnable);
        Pipeline& addDepthStencilState(const bool depthTestEnable,
            const bool depthWriteEnable,
            const vk::CompareOp compareOp,
            const bool depthBoundsTestEnable,
            const bool stencilTestEnable,
            const vk::StencilOpState front,
            const vk::StencilOpState back,
            const float minDepthBounds,
            const float maxDepthBounds);
        Pipeline& addColorBlendState(const bool logicOpEnable,
            const vk::LogicOp logicOp,
            const std::vector<vk::PipelineColorBlendAttachmentState> attachments,
            const std::array<float, 4> blendConstants);       // always 4
        Pipeline& addDynamicState(const std::vector<vk::DynamicState> dynamicStates);
        Pipeline& setLayout(const vk::PipelineLayout& layout);
        Pipeline& setRenderPass(const vk::RenderPass& renderPass);
        Pipeline& setSubpassIndex(const uint32_t subpassIndex);
        Pipeline& setBaseHandleAndIndex(const vk::Pipeline& baseHandle, const int32_t baseIndex);
        void create();

        //vk::PipelineCache generateCache();
        const vk::Pipeline& getPipeline() const;
        const vk::PipelineLayout& getLayout() const;
        void destroy();
        ~Pipeline();
    private:
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos;
        vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        vk::PipelineTessellationStateCreateInfo tessellationInfo;
        vk::PipelineViewportStateCreateInfo viewportInfo;
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
        vk::PipelineDynamicStateCreateInfo dynamicInfo;
        vk::PipelineLayout layout;
        vk::RenderPass renderPass;
        uint32_t subpass;
        vk::Pipeline baseHandle;
        int32_t baseIndex;
        vk::GraphicsPipelineCreateInfo pipelineInfo;

        vk::Pipeline pipeline;
    };
}

#endif