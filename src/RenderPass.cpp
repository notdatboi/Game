#include"../include/RenderPass.hpp"

namespace spk
{
    void RenderPass::create(const std::vector<vk::AttachmentDescription>& attachments,
        const std::vector<vk::SubpassDescription>& subpasses,
        const std::vector<vk::SubpassDependency>& dependencies)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        recordingBufferIndex = 0;
        vk::RenderPassCreateInfo renderPassInfo;
        renderPassInfo.setAttachmentCount(attachments.size())
            .setPAttachments(attachments.data())
            .setSubpassCount(subpasses.size())
            .setPSubpasses(subpasses.data())
            .setDependencyCount(dependencies.size())
            .setPDependencies(dependencies.data());
        if(logicalDevice.createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess) throw std::runtime_error("Failed to create render pass!\n");
    }

    RenderPass& RenderPass::addFramebuffer(const std::vector<vk::ImageView>& attachments, const vk::Extent2D extent)
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const vk::CommandPool& pool = system::Executives::getInstance()->getPool();
        vk::FramebufferCreateInfo framebufferInfo;
        framebufferInfo.setRenderPass(renderPass)
            .setAttachmentCount(attachments.size())
            .setPAttachments(attachments.data())
            .setWidth(extent.width)
            .setHeight(extent.height)
            .setLayers(1);
        framebuffers.push_back(vk::Framebuffer());
        if(logicalDevice.createFramebuffer(&framebufferInfo, nullptr, &framebuffers.back()) != vk::Result::eSuccess) throw std::runtime_error("Failed to add framebuffer!\n");

        frameCommandBuffers.push_back(vk::CommandBuffer());
        vk::CommandBufferAllocateInfo cbInfo;
        cbInfo.setCommandBufferCount(1)
            .setCommandPool(pool)
            .setLevel(vk::CommandBufferLevel::ePrimary);
        if(logicalDevice.allocateCommandBuffers(&cbInfo, &frameCommandBuffers.back()) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate command buffer!\n");

        return *this;
    }

    RenderPass& RenderPass::beginRecording(const uint32_t index, const vk::Fence& waitFence)
    {
        if(index > frameCommandBuffers.size() - 1) throw std::out_of_range("Command buffer index is out of range!\n");
        recordingBufferIndex = index;
        vk::CommandBuffer& currentCB = frameCommandBuffers[recordingBufferIndex];
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setPInheritanceInfo(nullptr);

        if(waitFence)
        {
            const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
            if(logicalDevice.waitForFences(1, &waitFence, true, ~0U) != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for fence!\n");
        }

        if(currentCB.begin(&beginInfo) != vk::Result::eSuccess) throw std::runtime_error("Failed to begin command buffer!\n");

        return *this;
    }

    RenderPass& RenderPass::beginPass(const std::vector<vk::ClearValue> clearValues, const vk::Rect2D renderArea)
    {
        vk::CommandBuffer& currentCB = frameCommandBuffers[recordingBufferIndex];

        vk::RenderPassBeginInfo renderBeginInfo;
        renderBeginInfo.setRenderPass(renderPass)
            .setFramebuffer(framebuffers[recordingBufferIndex])
            .setRenderArea(renderArea)
            .setClearValueCount(clearValues.size())
            .setPClearValues(clearValues.data());

        currentCB.beginRenderPass(&renderBeginInfo, vk::SubpassContents::eSecondaryCommandBuffers);

        return *this;
    }

    RenderPass& RenderPass::resetQueries(const vk::QueryPool& pool, const uint32_t firstQueryIndex, const uint32_t queryCount)
    {
        frameCommandBuffers[recordingBufferIndex].resetQueryPool(pool, firstQueryIndex, queryCount);

        return *this;
    }

    RenderPass& RenderPass::beginQuery(const vk::QueryPool& pool, const uint32_t queryIndex, const vk::QueryControlFlags flags)
    {
        frameCommandBuffers[recordingBufferIndex].beginQuery(pool, queryIndex, flags);

        return *this;
    }

    RenderPass& RenderPass::endQuery(const vk::QueryPool& pool, const uint32_t queryIndex)
    {
        frameCommandBuffers[recordingBufferIndex].endQuery(pool, queryIndex);

        return *this;
    }

    RenderPass& RenderPass::nextSubpass()
    {
        vk::CommandBuffer& currentCB = frameCommandBuffers[recordingBufferIndex];
        currentCB.nextSubpass(vk::SubpassContents::eSecondaryCommandBuffers);

        return *this;
    }

    RenderPass& RenderPass::executeCommandBuffer(const vk::CommandBuffer& subpassCommandBuffer)
    {
        vk::CommandBuffer& currentCB = frameCommandBuffers[recordingBufferIndex];
        currentCB.executeCommands(1, &subpassCommandBuffer);

        return *this;
    }

    RenderPass& RenderPass::endPass()
    {
        vk::CommandBuffer& currentCB = frameCommandBuffers[recordingBufferIndex];
        currentCB.endRenderPass();

        return *this;
    }

    RenderPass& RenderPass::endRecording()
    {
        vk::CommandBuffer& currentCB = frameCommandBuffers[recordingBufferIndex];
        currentCB.end();
        
        return *this;
    }

    RenderPass& RenderPass::resetCommandBuffer(const uint32_t index, const bool releaseResources)
    {
        if(index > frameCommandBuffers.size() - 1) throw std::out_of_range("Command buffer index is out of range!\n");
        frameCommandBuffers[index].reset(releaseResources ? vk::CommandBufferResetFlagBits::eReleaseResources : vk::CommandBufferResetFlags());

        return *this;
    }

    const vk::RenderPass& RenderPass::getRenderPass() const
    {
        return renderPass;
    }

    const vk::Framebuffer& RenderPass::getFramebuffer(const uint32_t index) const
    {
        return framebuffers[index];
    }

    const vk::CommandBuffer& RenderPass::getCommandBuffer(const uint32_t index) const
    {
        return frameCommandBuffers[index];
    }

    //vk::CommandBuffer& RenderPass::getCommandBuffer(const uint32_t index)
    //{
    //    return frameCommandBuffers[index];
    //}

    const uint32_t RenderPass::getFramebufferCount() const
    {
        return framebuffers.size();
    }

    void RenderPass::destroy()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const vk::CommandPool& pool = system::Executives::getInstance()->getPool();
        for(auto& framebuffer : framebuffers)
        {
            if(framebuffer)
            {
                logicalDevice.destroyFramebuffer(framebuffer, nullptr);
                framebuffer = vk::Framebuffer();
            }
        }
        for(auto& commandBuffer : frameCommandBuffers)
        {
            if(commandBuffer)
            {
                logicalDevice.freeCommandBuffers(pool, 1, &commandBuffer);
                commandBuffer = vk::CommandBuffer();
            }
        }
        if(renderPass)
        {
            logicalDevice.destroyRenderPass(renderPass, nullptr);
            renderPass = vk::RenderPass();
        }
    }

    RenderPass::~RenderPass()
    {
        destroy();
    }
}