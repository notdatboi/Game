#include<RenderWindow.hpp>

namespace spk
{
    RenderWindow::RenderWindow()
    {
    }

    void RenderWindow::create(const std::string& name, const vk::Extent2D extent/*, bool fullscreen*/)
    {
        const auto& instance = system::System::getInstance()->getvkInstance();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(extent.width, extent.height, name.c_str(), nullptr, nullptr);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        VkSurfaceKHR tmp;
        if(glfwCreateWindowSurface(instance, window, nullptr, &tmp) != VK_SUCCESS) throw std::runtime_error("Failed to create window.\n");
        surface = tmp;

        swapchainImageFormat = vk::Format::eR8G8B8A8Snorm;
        swapchain.create(surface, 3, swapchainImageFormat, extent, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor, vk::PresentModeKHR::eFifo, true);
    }

    const GLFWwindow* RenderWindow::getGLFWWindow() const
    {
        return window;
    }

    RenderWindow::~RenderWindow()
    {
        if(window)
        {
            glfwDestroyWindow(window);
        }
        // do we need to do anything to surface?
    }
    
    const vk::AttachmentDescription RenderWindow::getAttachmentDescription(const bool loadPreviousContent, const bool saveContent) const
    {
        vk::AttachmentDescription description;
        description.setFormat(swapchainImageFormat)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(loadPreviousContent ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear)
            .setStoreOp(saveContent ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
        return description;
    }

    const std::vector<vk::ImageView> RenderWindow::getViews() const
    {
        return swapchain.getImageViews();
    }
}