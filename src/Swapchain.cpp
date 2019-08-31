#include<Swapchain.hpp>

Swapchain::Swapchain()
{

}

void Swapchain::create(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const uint32_t queueFamilyIndexCount, const uint32_t* queueFamilyIndices)
{
    Array<VkSurfaceFormatKHR> formats;
    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
    formats.create(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, formats.getPtr());
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    uint32_t imageCount = 3;
    if(imageCount > surfaceCapabilities.maxImageCount) imageCount = surfaceCapabilities.maxImageCount;
    if(imageCount < surfaceCapabilities.minImageCount) imageCount = surfaceCapabilities.minImageCount;
    VkImageUsageFlags neededUsageFlags = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if((surfaceCapabilities.supportedUsageFlags & neededUsageFlags) != neededUsageFlags) reportError("Invalid surface.\n");

    VkSwapchainCreateInfoKHR swapchainInfo;
    swapchainInfo = 
    {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        surface,
        imageCount,
        formats[0].format,
        formats[0].colorSpace,
        surfaceCapabilities.currentExtent,
        1,
        neededUsageFlags,
        (queueFamilyIndexCount == 1) ? VkSharingMode::VK_SHARING_MODE_EXCLUSIVE : VkSharingMode::VK_SHARING_MODE_CONCURRENT,
        queueFamilyIndexCount,
        queueFamilyIndices,
        surfaceCapabilities.currentTransform,
        VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR,
        true,
        0
    };

    vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &swapchain);
}