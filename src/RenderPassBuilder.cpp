#include<RenderPassBuilder.hpp>

namespace spk
{
    RenderPassBuilder::RenderPassBuilder(){}

    void RenderPassBuilder::addAttachment(const vk::AttachmentDescription attachment)
    {

    }

    void RenderPassBuilder::useAttachmentAsRenderTarget(const uint32_t attachmentIndex, const uint32_t subpassIndex);
    void RenderPassBuilder::useAttachmentAsDepthMap(const uint32_t attachmentIndex, const uint32_t subpassIndex);
    void RenderPassBuilder::useAttachmentAsInput(const uint32_t attachmentIndex, const uint32_t subpassIndex);
    void RenderPassBuilder::useAttachmentAsPreserved(const uint32_t attachmentIndex, const uint32_t subpassIndex);
    void RenderPassBuilder::addDependency(const vk::SubpassDependency dependency);
    void RenderPassBuilder::clearSettings();
    vk::RenderPass RenderPassBuilder::buildRenderPass();
    RenderPassBuilder::~RenderPassBuilder();
}