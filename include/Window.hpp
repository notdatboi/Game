#ifndef WINDOW_HPP
#define WINDOW_HPP
#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<Utils.hpp>

class Window
{
public:
    Window();
    Window(const char* title, const VkExtent2D& size);
    void create(const char* title, const VkExtent2D& size);
    GLFWwindow* getWindow();
    const char** getVulkanExtensions(uint32_t& count) const;
    VkSurfaceKHR getVulkanSurface(const VkInstance& instance) const;
    VkExtent2D getWindowSize() const;
    ~Window();
private:
    VkExtent2D size;
    GLFWwindow* window;
};

#endif