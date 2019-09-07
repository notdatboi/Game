#ifndef RENDERER_HPP
#define RENDERER_HPP
#include<System.hpp>
#include<Swapchain.hpp>
#include<RenderPassHolder.hpp>
#include<CommandPool.hpp>
#include<SynchronizationPool.hpp>
#include<ImagePool.hpp>

struct RenderSyncPrimitives
{
    uint32_t imageAcquired;
    uint32_t renderFinished;
};

class Renderer
{
public:
    Renderer();
    void createBase(const System* system, const CommandPool* commandPool, const SynchronizationPool* syncPool, const ImagePool* imagePool);
    const uint32_t getSwapchainImageCount() const;
    void createRenderPass(Array<uint32_t>&& depthAttachmentIndices, const VkFormat depthAttachmentFormat, Array<uint32_t>&& commandBufferIndices, Array<RenderSyncPrimitives>&& semaphoreIndices, Array<RenderSyncPrimitives>&& fenceIndices);
    void beginRendering();
    void endRendering();
    void destroy();
    ~Renderer();
private:
    const System* system;
    const CommandPool* commandPool;
    const SynchronizationPool* syncPool;
    const ImagePool* imagePool;
    Swapchain swapchain;
    RenderPassHolder renderPass;
    Array<uint32_t> depthAttachments;
    Array<uint32_t> commandBuffers;
    Array<RenderSyncPrimitives> semaphores;
    Array<RenderSyncPrimitives> fences;
    uint32_t currentSubmission = 0;
    uint32_t currentImage;
};

#endif