#include<RenderPassHolder.hpp>

RenderPassHolder::RenderPassHolder()
{

}

void RenderPassHolder::create(const System* system, const Array<VkAttachmentDescription>& attachments, const Array<VkSubpassDescription>& subpasses, const Array<VkSubpassDependency>& subpassDepenencies, const uint32_t framebufferCount)
{
    this->system = system;
    VkRenderPassCreateInfo renderPassInfo = 
    {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        nullptr,
        0,
        attachments.getSize(),
        attachments.getPtr(),
        subpasses.getSize(),
        subpasses.getPtr(),
        subpassDepenencies.getSize(),
        subpassDepenencies.getPtr()
    };
    checkResult(vkCreateRenderPass(system->getDevice(), &renderPassInfo, nullptr, &renderPass), "Failed to create render pass.\n");
    associatedFramebuffers.create(framebufferCount);
}

void RenderPassHolder::createFramebuffer(const uint32_t index, const Array<VkImageView>& attachments, const VkExtent2D& extent, const uint32_t layers = 1)
{
    VkFramebufferCreateInfo framebufferInfo = 
    {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        nullptr,
        0,
        renderPass,
        attachments.getSize(),
        attachments.getPtr(),
        extent.width,
        extent.height,
        layers
    };
    vkCreateFramebuffer(system->getDevice(), &framebufferInfo, nullptr, &associatedFramebuffers[index]);
}

const VkRenderPass& RenderPassHolder::getRenderPass() const
{
    return renderPass;
}

const VkFramebuffer RenderPassHolder::operator[](const uint32_t index) const
{
    return associatedFramebuffers[index];
}

void RenderPassHolder::destroy()
{
    for(auto ind = 0; ind < associatedFramebuffers.getSize(); ++ind)
    {
        if(associatedFramebuffers[ind])
        {
            vkDestroyFramebuffer(system->getDevice(), associatedFramebuffers[ind], nullptr);
            associatedFramebuffers[ind] = 0;
        }
    }
    associatedFramebuffers.clean();
    if(renderPass)
    {
        vkDestroyRenderPass(system->getDevice(), renderPass, nullptr);
        renderPass = 0;
    }
}

RenderPassHolder::~RenderPassHolder()
{
    destroy();
}