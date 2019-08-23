#ifndef SPARK_PIPELINE_HPP
#define SPARK_PIPELINE_HPP

#include<vector>
#include<ShaderSet.hpp>
#include<Mesh.hpp>

namespace spk
{
    // Setters, which are labeled as 'optional', are optional for pipeline creation
    class Pipeline
    {
    public:
        Pipeline();
        void setShaders(const ShaderSet& shaders);
        void setVertexDescription(const VertexDescription& description);
        void setInputAssemblyState(const vk::PrimitiveTopology topology, const bool primitiveRestartEnable = false);
        void setTessellationState(const uint32_t patchControlPoints);                       // only if topology is PatchList; that's why optional
        void setViewportState(const std::vector<vk::Viewport> viewports, const std::vector<vk::Rect2D> scissors);

        void setRasterizationState(const bool depthClampEnable,
            const bool rasterizedDiscardEnable,
            const vk::PolygonMode polygonMode,
            const vk::CullModeFlags cullMode,
            const vk::FrontFace frontFace,
            const bool depthBiasEnable,
            const float depthBiasConstantFactor,
            const float depthBiasClamp,
            const float depthBiasSlopeFactor,
            const float lineWidth);
        void setRasterizationState(const vk::PolygonMode polygonMode,
            const vk::CullModeFlags cullMode,
            const vk::FrontFace frontFace);

        void setMultisampleState(const vk::SampleCountFlagBits rasterizationSamples,
            const bool sampleShadingEnable,
            const float minSampleShading,
            const vk::SampleMask* sampleMask,
            const bool alphaToCoverageEnable,
            const bool alphaToOneEnable);                   // optional

        void setDepthStencilState(const bool depthTestEnable,
            const bool depthWriteEnable,
            const vk::CompareOp compareOp,
            const bool depthBoundsTestEnable,
            const bool stencilTestEnable,
            const vk::StencilOpState front,
            const vk::StencilOpState back,
            const float minDepthBounds,
            const float maxDepthBounds);
        void setDepthStencilState(const bool enable, const vk::CompareOp compareOp);

        void setColorBlendState(const bool logicOpEnable,
            const vk::LogicOp logicOp,
            const std::vector<vk::PipelineColorBlendAttachmentState> attachments,
            const std::array<float, 4> blendConstants);       // always 4
        void setColorBlendState(const uint32_t renderTargetCount);

        void setDynamicState(const std::vector<vk::DynamicState> dynamicStates);    // optional
        void setRenderPass(const vk::RenderPass& renderPass);
        void setSubpassIndex(const uint32_t subpassIndex);
        void setBaseHandleAndIndex(const vk::Pipeline& baseHandle, const int32_t baseIndex);    // optional
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