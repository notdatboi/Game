#include<Window.hpp>
#include<cstring>

Window::Window()
{
}

Window::Window(const char* title, const VkExtent2D& size)
{
    create(title, size);
}

void Window::create(const char* title, const VkExtent2D& size)
{
    this->size = size;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(size.width, size.height, title, nullptr, nullptr);
}

GLFWwindow* Window::getWindow()
{
    return window;
}

const char** Window::getVulkanExtensions(uint32_t& count) const
{
    return glfwGetRequiredInstanceExtensions(&count);
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