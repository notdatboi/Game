#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP
#include<vulkan/vulkan.h>
#include<Utils.hpp>

class Swapchain
{
public:
    Swapchain();
    void create(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices);
    ~Swapchain();
private:
    VkSwapchainKHR swapchain;
};

#endif