#include<Renderer.hpp>

Renderer::Renderer()
{
}

void Renderer::create(const Window& window, const std::vector<std::string>& sceneFilenames, const std::string& imagePath)
{
    VkPhysicalDeviceFeatures features = {};
    features.logicOp = VK_TRUE;
    system.create(window, true, features);
    uint32_t swapchainImgCount;
    swapchain.create(&system, swapchainImgCount);
    commandPool.create(&system, true);
    syncPool.create(&system);
    allocator = new SharedMemoryObjectManagementStrategy();
    allocator->create(&system, &syncPool, &commandPool);

    viewProj.view = glm::lookAt(glm::vec3(8, 5, 7), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    viewProj.projection = glm::perspective(glm::radians(60.0f), (float)swapchain.getExtent().width / swapchain.getExtent().height, 0.1f, 100.0f);
    allocator->allocateUniformBuffer(sizeof(viewProj), VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT | VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, viewProjBuffer, viewProjDescriptor);
    allocator->updateBuffer(&viewProj, viewProjBuffer);

    scenes.create(sceneFilenames.size());
    for(auto ind = 0; ind < sceneFilenames.size(); ++ind)
    {
        scenes[ind].setAllocator(allocator);
        scenes[ind].loadFromFile(imagePath, sceneFilenames[ind]);
    }

    depthAttachments.create(swapchainImgCount);
    for(auto ind = 0; ind < swapchainImgCount; ++ind)
    {
        allocator->allocateDepthMap(swapchain.getExtent(), depthAttachments[ind]);
    }

    allocator->load();
    allocator->update();

    for(auto ind = 0; ind < sceneFilenames.size(); ++ind)
    {
        scenes[ind].clearExtraResources();
    }

    createRenderPass();
    createPipelines();
}

Scene& Renderer::getScene(const uint32_t index)
{
    return scenes[index];
}

const Scene& Renderer::getScene(const uint32_t index) const
{
    return scenes[index];
}

const uint32_t Renderer::getSwapchainImageCount() const
{
    return swapchain.getImageCount();
}

void Renderer::createRenderPass()
{
    uint32_t swapchainImgCount = getSwapchainImageCount();
    VkFormat depthAttachmentFormat;
    VkImageTiling depthAttachmentTiling;
    allocator->pickDepthStencilFormat(depthAttachmentFormat, depthAttachmentTiling);
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
    Array<VkSubpassDependency> dependencies(1);
    dependencies[0] = 
    {
        VK_SUBPASS_EXTERNAL,
        0,
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VkAccessFlags(),
        VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };
    renderPass.create(&system, attachments, subpasses, dependencies, swapchainImgCount);

    commandBuffers.create(swapchainImgCount);
    semaphores.create(swapchainImgCount);
    fences.create(swapchainImgCount);
    uint32_t firstSemaphore = syncPool.getSemaphoreCount(), firstFence = syncPool.getFenceCount();
    syncPool.addSemaphores(swapchainImgCount * 2);
    syncPool.addFences(swapchainImgCount, false);
    syncPool.addFences(swapchainImgCount, false);
    uint32_t firstCommandBuffer = commandPool.getCurrentPoolSize();
    commandPool.addCommandBuffers(swapchainImgCount);
    commandPool.allocateCommandBuffers(firstCommandBuffer, swapchainImgCount, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    for(auto ind = 0; ind < swapchainImgCount; ++ind)
    {
        semaphores[ind].imageAcquired = firstSemaphore + ind * 2;
        semaphores[ind].renderFinished = firstSemaphore + ind * 2 + 1;
        fences[ind].imageAcquired = firstFence + ind;
        fences[ind].renderFinished = firstFence + ind + swapchainImgCount;
        commandBuffers[ind] = firstCommandBuffer + ind;
        Array<VkImageView> attachments = {swapchain.getView(ind), depthAttachments[ind].holder->getView(depthAttachments[ind].viewIndex)};
        renderPass.createFramebuffer(ind, attachments, swapchain.getExtent());
    }
}
#include<iostream>
void Renderer::beginRendering()
{
    allocator->update();
    commandPool.reset(commandBuffers[currentSubmission], true);
    currentImage = swapchain.acquireNextImage(syncPool.getSemaphore(semaphores[currentSubmission].imageAcquired), syncPool.getFence(fences[currentSubmission].imageAcquired));
    std::cout << "Current sub: " << currentSubmission << " Current image: " << currentImage << " Used subs: " << usedSubmissions << '\n';

    VkCommandBufferBeginInfo beginInfo = 
    {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        nullptr
    };

    VkImageSubresourceRange subresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1
    };

    vkBeginCommandBuffer(commandPool[commandBuffers[currentSubmission]], &beginInfo);
    ImageHolder::recordLayoutChangeCommands(commandPool[commandBuffers[currentSubmission]], (!(usedSubmissions & (1 << currentSubmission))) ? VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED : VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, swapchain.getImage(currentImage), subresource);

    VkRect2D renderArea;
    renderArea.extent = swapchain.getExtent();
    renderArea.offset = {0, 0};
    VkClearValue clearVals[2];
    clearVals[0].color = {0, 0, 0, 0};//{1 * (currentSubmission == 0), 1 * (currentSubmission == 1), 1 * (currentSubmission == 2), 1.0};
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
    vkCmdBeginRenderPass(commandPool[commandBuffers[currentSubmission]], &renderPassInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
}

void Renderer::renderSceneNode(const Scene::Node& node)
{
    static DrawableType prevType = DrawableType::DTCount;
    const VkCommandBuffer& commands = commandPool[commandBuffers[currentSubmission]];
    const uint32_t meshCount = node.getMeshes().getSize();
    for(auto meshInd = 0; meshInd < meshCount; ++meshInd)
    {
        const Mesh* mesh = node.getMeshes()[meshInd];
        const DrawableType type = mesh->getMaterial()->getType();
        const DescriptorInfo& nodeModelDescriptor = node.getModelMatrixDescriptor();
        if(prevType != type)
        {
            vkCmdBindPipeline(commands, 
                VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, 
                pipelinePool[type]);

            const auto& matDescriptors = mesh->getMaterial()->getDescriptorInfos();
            Array<VkDescriptorSet> sets(2 + matDescriptors.getSize());
            sets[0] = (*viewProjDescriptor.pool)[viewProjDescriptor.setIndex];
            sets[1] = (*nodeModelDescriptor.pool)[nodeModelDescriptor.setIndex];
            for(auto ind = 2; ind < sets.getSize(); ++ind)
            {
                sets[ind] = (*matDescriptors[ind - 2].pool)[matDescriptors[ind - 2].setIndex];
            }

            vkCmdBindDescriptorSets(commands, 
                VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, 
                allocator->getPipelineLayout(type),
                0,
                sets.getSize(),
                sets.getPtr(),
                0,
                nullptr);
        }
        else
        {
            const auto& matDescriptors = mesh->getMaterial()->getDescriptorInfos();
            Array<VkDescriptorSet> sets(1 + matDescriptors.getSize());
            sets[0] = (*nodeModelDescriptor.pool)[nodeModelDescriptor.setIndex];
            for(auto ind = 1; ind < sets.getSize(); ++ind)
            {
                sets[ind] = (*matDescriptors[ind - 1].pool)[matDescriptors[ind - 1].setIndex];
            }
            vkCmdBindDescriptorSets(commands, 
                VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, 
                allocator->getPipelineLayout(type),
                1,
                sets.getSize(),
                sets.getPtr(),
                0,
                nullptr);
        }
        const BufferInfo& vb = mesh->getVertexBuffer(), ib = mesh->getIndexBuffer();
        vkCmdBindVertexBuffers(commands, 0, 1, &(*vb.holder)[vb.index], &vb.offset);
        vkCmdBindIndexBuffer(commands, (*ib.holder)[ib.index], ib.offset, VkIndexType::VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commands, mesh->getIndexCount(), 1, 0, 0, 0);
        prevType = type;
    }

    for(const auto& kvPair : node.getChildrenNodes())
    {
        renderSceneNode(kvPair.second);
    }

    prevType = DrawableType::DTCount;
}

void Renderer::endRendering()
{
    const VkCommandBuffer& commands = commandPool[commandBuffers[currentSubmission]];
    vkCmdEndRenderPass(commands);
    vkEndCommandBuffer(commands);

    VkPipelineStageFlags dstFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit = 
    {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        nullptr,
        1,
        &syncPool.getSemaphore(semaphores[currentSubmission].imageAcquired),
        &dstFlags,
        1,
        &commands,
        1,
        &syncPool.getSemaphore(semaphores[currentSubmission].renderFinished)
    };

    syncPool.waitForFences(fences[currentSubmission].imageAcquired);
    syncPool.resetFences(fences[currentSubmission].imageAcquired);
    checkResult(vkQueueSubmit(system.getGraphicsQueue().queue, 1, &submit, syncPool.getFence(fences[currentSubmission].renderFinished)), "Queue submission failed.\n");

    VkResult presentResult;
    VkPresentInfoKHR present = 
    {
        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        nullptr,
        1,
        &syncPool.getSemaphore(semaphores[currentSubmission].renderFinished),
        1,
        &swapchain.getSwapchain(),
        &currentImage,
        &presentResult
    };

    syncPool.waitForFences(fences[currentSubmission].renderFinished);
    syncPool.resetFences(fences[currentSubmission].renderFinished);

    checkResult(vkQueuePresentKHR(system.getPresentQueue().queue, &present), "Present submission failed.\n");
    checkResult(presentResult, "Image presentation failed.\n");

    if(!(usedSubmissions & (1 << currentSubmission))) usedSubmissions |= (1 << currentSubmission);
    currentSubmission = (currentSubmission + 1) % swapchain.getImageCount();
}

void Renderer::createPipelines()
{
    textured.create(2);
    notTextured.create(2);
    normalMapped.create(2);
    textured[0].create(&system, "RenderSystem/shaders/Textured.vert");
    textured[1].create(&system, "RenderSystem/shaders/Textured.frag");
    notTextured[0].create(&system, "RenderSystem/shaders/NotTextured.vert");
    notTextured[1].create(&system, "RenderSystem/shaders/NotTextured.frag");
    normalMapped[0].create(&system, "RenderSystem/shaders/NormalMapped.vert");
    normalMapped[1].create(&system, "RenderSystem/shaders/NormalMapped.frag");
    Array<ShaderStageInfo> texturedInfos(textured.getSize()), notTexturedInfos(notTextured.getSize()), normalMappedInfos(normalMapped.getSize());
    for(auto ind = 0; ind < textured.getSize(); ++ind) texturedInfos[ind] = textured[ind].getShader();
    for(auto ind = 0; ind < notTextured.getSize(); ++ind) notTexturedInfos[ind] = notTextured[ind].getShader();
    for(auto ind = 0; ind < normalMapped.getSize(); ++ind) normalMappedInfos[ind] = normalMapped[ind].getShader();

    Array<VkViewport> viewports(1);
    viewports[0] = 
    {
        0,
        0,
        static_cast<float>(swapchain.getExtent().width),
        static_cast<float>(swapchain.getExtent().height),
        0,
        1
    };
    Array<VkRect2D> scissors(1);
    scissors[0] = 
    {
        {0, 0},
        {static_cast<uint32_t>(viewports[0].width), static_cast<uint32_t>(viewports[0].height)}
    };
    Array<VkVertexInputBindingDescription> bindings;
    Array<VkVertexInputAttributeDescription> attributes;
    VertexBufferStandard vbs;
    VertexBufferNotTextured vbnt;
    VertexBufferWithNormalMap vbnm;
    Array<VkPipelineColorBlendAttachmentState> colorBlendStates(1);
    colorBlendStates[0] = 
    {
        VK_FALSE,
        VkBlendFactor::VK_BLEND_FACTOR_SRC_COLOR,
        VkBlendFactor::VK_BLEND_FACTOR_DST_COLOR,
        VkBlendOp::VK_BLEND_OP_ADD,
        VkBlendFactor::VK_BLEND_FACTOR_SRC_ALPHA,
        VkBlendFactor::VK_BLEND_FACTOR_DST_ALPHA,
        VkBlendOp::VK_BLEND_OP_ADD,
        VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT
    };
    pipelinePool.create(&system, DrawableType::DTCount);
    PipelineInfoBuilder infoBuilder;
    infoBuilder.setShaderStages(texturedInfos);
    vbs.getGraphicsPipelineVertexInputState(bindings, attributes);
    infoBuilder.setVertexInputState(true, bindings, attributes);
    infoBuilder.setInputAssemblyState(true);
    infoBuilder.setTessellationState(false);
    infoBuilder.setViewportState(true, viewports, scissors);
    infoBuilder.setRasterizationState(VK_FALSE, VkPolygonMode::VK_POLYGON_MODE_FILL, VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT, VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE);
    infoBuilder.setMultisampleState();
    infoBuilder.setDepthStencilState();
    infoBuilder.setColorBlendState(true, VK_FALSE, VkLogicOp(), colorBlendStates);
    infoBuilder.setDynamicState(false);
    infoBuilder.setLayout(&allocator->getPipelineLayout(DrawableType::DTTextured));
    infoBuilder.setRenderPass(&renderPass.getRenderPass(), 0);

    pipelinePool.createPipeline(DrawableType::DTTextured, infoBuilder.generatePipelineInfo());

    infoBuilder.setShaderStages(notTexturedInfos);
    vbnt.getGraphicsPipelineVertexInputState(bindings, attributes);
    infoBuilder.setVertexInputState(true, bindings, attributes);
    infoBuilder.setLayout(&allocator->getPipelineLayout(DrawableType::DTNotTextured));

    pipelinePool.createPipeline(DrawableType::DTNotTextured, infoBuilder.generatePipelineInfo());

    infoBuilder.setShaderStages(normalMappedInfos);
    vbnm.getGraphicsPipelineVertexInputState(bindings, attributes);
    infoBuilder.setVertexInputState(true, bindings, attributes);
    infoBuilder.setLayout(&allocator->getPipelineLayout(DrawableType::DTTexturedWithNormalMap));

    pipelinePool.createPipeline(DrawableType::DTTexturedWithNormalMap, infoBuilder.generatePipelineInfo());
}

void Renderer::destroy()
{
    if(system.getDevice()) vkDeviceWaitIdle(system.getDevice());
    else return;
    swapchain.destroy();
    renderPass.destroy();
    pipelinePool.destroy();
    //for(auto ind = 0; ind < textured.getSize(); ++ind) textured[ind].destroy();
    textured.clear();
    //for(auto ind = 0; ind < notTextured.getSize(); ++ind) notTextured[ind].destroy();
    notTextured.clear();
    //for(auto ind = 0; ind < normalMapped.getSize(); ++ind) normalMapped[ind].destroy();
    normalMapped.clear();
    commandPool.destroy();
    syncPool.destroy();
    depthAttachments.clear();
    commandBuffers.clear();
    semaphores.clear();
    fences.clear();
    scenes.clear();
    allocator->destroy();
    delete allocator;

    system.destroy();
}

Renderer::~Renderer()
{
    destroy();
}