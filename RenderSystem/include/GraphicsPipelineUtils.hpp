#ifndef GRAPHICS_PIPELINE_UTILS_HPP
#define GRAPHICS_PIPELINE_UTILS_HPP
#include<System.hpp>

class PipelineInfoBuilder                   // all arrays must not be deleted while builder may use them
{
public:
    PipelineInfoBuilder();
    void setShaderStages(const Array<ShaderStageInfo>& stages);
    void setVertexInputState(const bool enable = true, const Array<VkVertexInputBindingDescription>& bindings = {}, const Array<VkVertexInputAttributeDescription>& attributes = {});
    void setInputAssemblyState(const bool enable = true, const VkPrimitiveTopology topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, const VkBool32 enablePrimitiveRestart = false);
    void setTessellationState(const bool enable = true, const uint32_t controlPoints = 3);
    void setViewportState(const bool enable = true, const Array<VkViewport>& viewports = {}, const Array<VkRect2D>& scissors = {});
    void setRasterizationState(const VkBool32 enableDepthClamp = VK_FALSE, const VkPolygonMode polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL, const VkCullModeFlags culling = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT, const VkFrontFace frontFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE, const VkBool32 enableDepthBias = VK_FALSE, const float constantFactor = 0, const float clamp = 0, const float slopeFactor = 0);
    void setMultisampleState(const bool enable = true, const VkSampleCountFlagBits samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT, const VkBool32 enableSampleShading = VK_FALSE, const float minSampleShading = 0, const VkSampleMask* sampleMask = nullptr, const VkBool32 alphaToCoverage = VK_FALSE, const VkBool32 alphaToOne = VK_FALSE);
    void setDepthStencilState(const bool enable = true, const VkBool32 enableDepthTest = VK_TRUE, const VkBool32 enableDepthWrite = VK_TRUE, const VkCompareOp compareOp = VkCompareOp::VK_COMPARE_OP_LESS, const VkBool32 enableDepthBoundsTest = VK_FALSE, const VkBool32 enableStencilTest = VK_FALSE, const VkStencilOpState& front = VkStencilOpState(), const VkStencilOpState& back = VkStencilOpState(), const float minDepthBounds = 0, const float maxDepthBounds = 1);
    void setColorBlendState(const bool enable = true, const VkBool32 enableLogicOp = VK_FALSE, const VkLogicOp logicOp = VkLogicOp::VK_LOGIC_OP_CLEAR, const Array<VkPipelineColorBlendAttachmentState>& attachments = Array<VkPipelineColorBlendAttachmentState>());
    void setDynamicState(const bool enable = true, const Array<VkDynamicState>& dynamicStates = Array<VkDynamicState>());
    void setLayout(const VkPipelineLayout* layout);
    void setRenderPass(const VkRenderPass* renderPass, const uint32_t subpass);
    void setBasePipeline(const VkPipeline* basePipeline, const int32_t basePipelineIndex);
    const VkGraphicsPipelineCreateInfo generatePipelineInfo() const;
    ~PipelineInfoBuilder();
private:
    Array<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    VkPipelineTessellationStateCreateInfo tessellationState;
    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineRasterizationStateCreateInfo rasterizationState;
    VkPipelineMultisampleStateCreateInfo multisampleState;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkGraphicsPipelineCreateInfo info;
};

class PipelinePool
{
public:
    PipelinePool();
    void create(const System* system, const uint32_t pipelineCount);
    void createPipeline(const uint32_t index, const VkGraphicsPipelineCreateInfo& pipelineInfo, const VkPipelineCache& cache = VkPipelineCache());
    const VkPipeline& operator[](const uint32_t index) const;
    void destroy();
    ~PipelinePool();
private:
    const System* system;
    Array<VkPipeline> pipelines;
};

#endif