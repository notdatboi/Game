#ifndef RENDER_PASS_HOLDER_HPP
#define RENDER_PASS_HOLDER_HPP
#include<System.hpp>

class RenderPassHolder
{
public:
    RenderPassHolder();
    void create(const System* system, const Array<VkAttachmentDescription>& attachments, const Array<VkSubpassDescription>& subpasses, const Array<VkSubpassDependency>& subpassDepenencies, const uint32_t framebufferCount);
    void createFramebuffer(const uint32_t index, const Array<VkImageView>& attachments, const VkExtent2D& extent, const uint32_t layers = 1);
    const VkRenderPass& getRenderPass() const;
    const VkFramebuffer operator[](const uint32_t index) const;
    void destroy();
    ~RenderPassHolder();
private:
    const System* system;
    VkRenderPass renderPass;
    Array<VkFramebuffer> associatedFramebuffers;
};

#endif