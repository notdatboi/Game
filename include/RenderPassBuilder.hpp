#ifndef SPARK_RENDER_PASS_BUILDER_HPP
#define SPARK_RENDER_PASS_BUILDER_HPP
#include<System.hpp>

namespace spk
{
    class RenderPassBuilder
    {
    public:
        RenderPassBuilder();
        void addAttachment(const vk::AttachmentDescription attachment);
        void useAttachmentAsRenderTarget(const uint32_t attachmentIndex, const uint32_t subpassIndex);
        void useAttachmentAsDepthMap(const uint32_t attachmentIndex, const uint32_t subpassIndex);
        void useAttachmentAsInput(const uint32_t attachmentIndex, const uint32_t subpassIndex);
        void useAttachmentAsPreserved(const uint32_t attachmentIndex, const uint32_t subpassIndex);
        void addDependency(const vk::SubpassDependency dependency);
        void clearSettings();
        vk::RenderPass buildRenderPass();
        ~RenderPassBuilder();
    private:
        std::vector<vk::AttachmentDescription> attachments;
        //std::vec
    };
}

#endif