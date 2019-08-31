#include<Window.hpp>

Window::Window()
{
    VkExtent2D extent = {640, 480};
    create("", extent);
}

Window::Window(const char* title, const VkExtent2D& size)
{
    create(title, size);
}

void Window::create(const char* title, const VkExtent2D& size)
{
    this->size = size;
    window = glfwCreateWindow(size.width, size.height, title, nullptr, nullptr);
}

GLFWwindow* Window::getWindow()
{
    return window;
}

Array<const char*> Window::getVulkanExtensions() const
{
    Array<const char*> result;
    uint32_t extensionCount;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    result.create(extensionCount, extensions);
    return result;
}

VkSurfaceKHR Window::getVulkanSurface(const VkInstance& instance) const
{
    VkSurfaceKHR surface;
    checkResult(glfwCreateWindowSurface(instance, window, nullptr, &surface), "Failed to create surface.\n");
    return surface;
}

VkExtent2D Window::getWindowSize() const
{
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    VkExtent2D size;
    size.width = w;
    size.height = h;
    return size;
}

Window::~Window()
{
    if(window)
    {
        glfwDestroyWindow(window);
    }
}