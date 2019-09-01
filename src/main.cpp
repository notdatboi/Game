#include<Swapchain.hpp>

int main()
{
    glfwInit();
    Window window;
    window.create("TEST", {800, 600});
    System system;
    VkPhysicalDeviceFeatures features{};
    system.create(window, true, features);
    Swapchain swapchain;
    swapchain.create(&system);
    swapchain.destroy();
    system.destroy();
    glfwTerminate();
}