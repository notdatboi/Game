#ifndef SYSTEM_HPP
#define SYSTEM_HPP
#include<vulkan/vulkan.h>
#include<Window.hpp>

struct QueueInfo
{
    VkQueue queue;
    uint32_t familyIndex;
};

class System
{
public:
    System();
    void create(const Window& window, const bool enableDebug, const VkPhysicalDeviceFeatures& enabledFeatures);
    const VkPhysicalDevice& getPhysicalDevice() const;
    const VkSurfaceKHR& getSurface() const;
    const VkSurfaceCapabilitiesKHR getSurfaceCapabilities() const;
    const VkDevice& getDevice() const;
    const QueueInfo& getPresentQueue() const;
    const QueueInfo& getGraphicsQueue() const;
    void destroy();
    ~System();
private:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    QueueInfo graphicsQueue;
    QueueInfo presentQueue;
    VkDebugUtilsMessengerEXT debugMessenger;

    void createInstance(const char** customExtensions, const uint32_t& extensionCount, const bool enableDebug);
    void createDebugMessenger();
    void pickPhysicalDevice();
    void pickQueueFamilies();
    void createDevice(const VkPhysicalDeviceFeatures& enabledFeatures);
    void obtainQueues();

    static VKAPI_ATTR VkBool32 VKAPI_CALL callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData);
};

#endif