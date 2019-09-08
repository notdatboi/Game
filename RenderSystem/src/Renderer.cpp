#include<Renderer.hpp>

Renderer::Renderer()
{
}

void Renderer::createBase(const System* system, const CommandPool* commandPool, const SynchronizationPool* syncPool, const ImagePool* imagePool)
{
    this->system = system;
    this->commandPool = commandPool;
    this->syncPool = syncPool;
    this->imagePool = imagePool;
}

const uint32_t Renderer::getSwapchainImageCount() const
{
    swapchain.getImageCount();
}

void Renderer::createRenderPass(Array<uint32_t>&& depthAttachmentIndices, const VkFormat depthAttachmentFormat, Array<uint32_t>&& commandBufferIndices, Array<RenderSyncPrimitives>&& semaphoreIndices, Array<RenderSyncPrimitives>&& fenceIndices)
{
    uint32_t swapchainImgCount;
    swapchain.create(system, swapchainImgCount);
    Array<VkAttachmentDescription> attachments(2);
    attachments[0] = 
    {
        0,
        swapchain.getFormat(),
        VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
        VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR,
        VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE,
        VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
        VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    attachments[1] = 
    {
        0,
        depthAttachmentFormat,
        VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
        VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR,
        VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE,
        VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
        VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthRef, colorRef;
    colorRef = 
    {
        0,
        VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    depthRef = 
    {
        1,
        VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };
    Array<VkSubpassDescription> subpasses(1);
    subpasses[0] = 
    {
        0,
        VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
        0,
        nullptr,
        1,
        &colorRef,
        nullptr,
        &depthRef,
        0,
        nullptr
    };
    Array<VkSubpassDependency> dependencies;
    renderPass.create(system, attachments, subpasses, dependencies, swapchainImgCount);
    depthAttachments = depthAttachmentIndices;
    commandBuffers = commandBufferIndices;
    semaphores = semaphoreIndices;
    fences = fenceIndices;
    for(auto ind = 0; ind < swapchainImgCount; ++ind)
    {
        Array<VkImageView> attachments = {swapchain.getView(ind), (*imagePool)[depthAttachments[ind]].view};
        renderPass.createFramebuffer(ind, attachments, swapchain.getExtent());
    }
}

void Renderer::beginRendering()
{
    syncPool->waitForFences(fences[currentSubmission].renderFinished);
    syncPool->resetFences(fences[currentSubmission].renderFinished);
    currentImage = swapchain.acquireNextImage(syncPool->getSemaphore(semaphores[currentSubmission].imageAcquired), syncPool->getFence(fences[currentSubmission].imageAcquired));

    VkCommandBufferBeginInfo beginInfo = 
    {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        nullptr
    };

    vkBeginCommandBuffer((*commandPool)[commandBuffers[currentSubmission]], &beginInfo);

    VkRect2D renderArea;
    renderArea.extent = swapchain.getExtent();
    renderArea.offset = {0, 0};
    VkClearValue clearVals[2];
    clearVals[0].color = {0, 0, 0, 1.0};
    clearVals[1].depthStencil = {1, 0};
    
    VkRenderPassBeginInfo renderPassInfo = 
    {
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        nullptr,
        renderPass.getRenderPass(),
        renderPass[currentImage],
        renderArea,
        2,
        clearVals
    };
    vkCmdBeginRenderPass((*commandPool)[commandBuffers[currentSubmission]], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::endRendering()
{
    const VkCommandBuffer& commands = (*commandPool)[commandBuffers[currentSubmission]];
    vkCmdEndRenderPass(commands);
    vkEndCommandBuffer(commands);

    VkPipelineStageFlags dstFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit = 
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        1,
        &syncPool->getSemaphore(semaphores[currentSubmission].imageAcquired),
        &dstFlags,
        1,
        &commands,
        1,
        &syncPool->getSemaphore(semaphores[currentSubmission].renderFinished)
    };

    syncPool->waitForFences(fences[currentSubmission].imageAcquired);
    syncPool->resetFences(fences[currentSubmission].imageAcquired);
    checkResult(vkQueueSubmit(system->getPresentQueue().queue, 1, &submit, syncPool->getFence(fences[currentSubmission].renderFinished)), "Queue submission failed.\n");

    VkResult presentResult;
    VkPresentInfoKHR present = 
    {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        &syncPool->getSemaphore(semaphores[currentSubmission].renderFinished),
        1,
        &swapchain.getSwapchain(),
        &currentImage,
        &presentResult
    };

    checkResult(vkQueuePresentKHR(system->getPresentQueue().queue, &present), "Present submission failed.\n");
    checkResult(presentResult, "Image presentation failed.\n");

    currentSubmission = (currentSubmission + 1) % swapchain.getImageCount();
}

void Renderer::destroy()
{
    renderPass.destroy();
    swapchain.destroy();
    depthAttachments.clean();
    commandBuffers.clean();
    semaphores.clean();
    fences.clean();
}

Renderer::~Renderer()
{
    destroy();
}