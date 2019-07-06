#ifndef SPARK_RENDER_PASS_HPP
#define SPARK_RENDER_PASS_HPP
#include"SparkIncludeBase.hpp"
#include"System.hpp"
#include"Executives.hpp"

namespace spk
{
    class RenderPass
    {
    public:
        void create(const std::vector<vk::AttachmentDescription>& attachments,
            const std::vector<vk::SubpassDescription>& subpasses,
            const std::vector<vk::SubpassDependency>& dependencies);
        RenderPass& addFramebuffer(const std::vector<vk::ImageView>& attachments, const vk::Extent2D extent);
        RenderPass& beginRecording(const uint32_t index, const uint32_t clearValueCount, const vk::Rect2D renderArea, const vk::Fence& waitFence = vk::Fence());
        RenderPass& nextSubpass(const vk::CommandBuffer& subpassCommandBuffer);          // vkCmdNextSubpass, then vkCmdExecuteCommands
        RenderPass& endRecording();
        RenderPass& resetCommandBuffer(const uint32_t index, const bool releaseResources);

        void destroy();
        ~RenderPass();

        // Getters
        const vk::RenderPass& getRenderPass() const;
        const vk::Framebuffer& getFramebuffer(const uint32_t index) const;
        const vk::CommandBuffer& getCommandBuffer(const uint32_t index) const;
        const uint32_t getFramebufferCount() const;
    private:
        vk::RenderPass renderPass;
        std::vector<vk::Framebuffer> framebuffers;
        std::vector<vk::CommandBuffer> frameCommandBuffers;
        uint32_t recordingBufferIndex;
    };
}

#endif