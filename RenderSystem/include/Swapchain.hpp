#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP
#include<vulkan/vulkan.h>
#include<System.hpp>

class Swapchain
{
public:
    Swapchain();
    void create(const System* system, uint32_t& imageCount, const VkFormat preferredFormat = VkFormat::VK_FORMAT_UNDEFINED);
    const uint32_t acquireNextImage(const VkSemaphore& signalSemaphore, const VkFence& signalFence) const;
    const VkFormat& getFormat() const;
    const VkExtent2D& getExtent() const;
    const uint32_t getImageCount() const;
    const VkSwapchainKHR& getSwapchain() const;
    const VkImageView& getView(const uint32_t index) const;
    const VkImage& getImage(const uint32_t index) const;
    void destroy();
    ~Swapchain();
private:
    VkSwapchainKHR swapchain;
    VkFormat format;
    VkExtent2D extent;
    Array<VkImage> images;
    Array<VkImageView> views;
    const System* system;
};

#endif