#ifndef SPARK_RENDER_WINDOW_HPP
#define SPARK_RENDER_WINDOW_HPP
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<Swapchain.hpp>
#include<RenderTarget.hpp>

namespace spk
{
    class RenderWindow : public RenderTarget
    {
    public:
        RenderWindow();
        void create(const std::string& name, const vk::Extent2D extent/*, bool fullscreen*/);
        const GLFWwindow* getGLFWWindow() const;
        ~RenderWindow();
    private:
        virtual const vk::AttachmentDescription getAttachmentDescription(const bool loadPreviousContent, const bool saveContent) const;
        virtual const std::vector<vk::ImageView> getViews() const;

        vk::Format swapchainImageFormat;
        Swapchain swapchain;
        vk::SurfaceKHR surface;
        GLFWwindow* window;
    };
}

#endif