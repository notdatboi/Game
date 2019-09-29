#ifndef RENDERER_HPP
#define RENDERER_HPP
#include<System.hpp>
#include<Swapchain.hpp>
#include<RenderPassHolder.hpp>
#include<SynchronizationPool.hpp>
#include<ObjectManagementStrategy.hpp>
#include<Scene.hpp>
#include<GraphicsPipelineUtils.hpp>
#include<Shader.hpp>

class Renderer
{
public:
    Renderer();
    void create(const Window& window, const std::vector<std::string>& sceneFilenames, const std::string& imagePath);
    Scene& getScene(const uint32_t index);
    const Scene& getScene(const uint32_t index) const;
    void beginRendering();
    void renderSceneNode(const Scene::Node& node);
    void endRendering();
    void destroy();
    ~Renderer();
private:
    struct RenderSyncPrimitives
    {
        uint32_t imageAcquired;
        uint32_t renderFinished;
    };
    struct ViewProjection
    {
        glm::mat4 view;
        glm::mat4 projection;
    } viewProj;
    BufferInfo viewProjBuffer;
    DescriptorInfo viewProjDescriptor;
    
    const uint32_t getSwapchainImageCount() const;
    void createRenderPass();
    void createPipelines();

    System system;
    Swapchain swapchain;
    RenderPassHolder renderPass;
    Array<Shader> textured; 
    Array<Shader> notTextured;
    Array<Shader> normalMapped;
    PipelinePool pipelinePool;
    CommandPool commandPool;
    SynchronizationPool syncPool;
    ObjectManagementStrategy* allocator;
    Array<ImageInfo> depthAttachments;
    Array<uint32_t> commandBuffers;
    Array<RenderSyncPrimitives> semaphores;
    Array<RenderSyncPrimitives> fences;
    Array<Scene> scenes;
    uint32_t currentSubmission = 0;
    uint32_t usedSubmissions = 0;
    uint32_t currentImage;
};

#endif