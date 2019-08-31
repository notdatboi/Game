#include<Swapchain.hpp>

Swapchain::Swapchain()
{

}

void Swapchain::create(const System* system)
{
    this->system = system;
    const VkPhysicalDevice& physicalDevice = system->getPhysicalDevice();
    const VkSurfaceKHR& surface = system->getSurface();
    Array<uint32_t> queueFamilyIndices;
    if(system->getGraphicsQueue().familyIndex == system->getPresentQueue().familyIndex)
    {
        queueFamilyIndices.create(1);
        queueFamilyIndices[0] = system->getGraphicsQueue().familyIndex;
    }
    else
    {
        queueFamilyIndices.create(2);
        queueFamilyIndices[0] = system->getGraphicsQueue().familyIndex;
        queueFamilyIndices[1] = system->getPresentQueue().familyIndex;
    }
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
        (queueFamilyIndices.getSize() == 1) ? VkSharingMode::VK_SHARING_MODE_EXCLUSIVE : VkSharingMode::VK_SHARING_MODE_CONCURRENT,
        queueFamilyIndices.getSize(),
        queueFamilyIndices.getPtr(),
        surfaceCapabilities.currentTransform,
        VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR,
        true,
        0
    };

    checkResult(vkCreateSwapchainKHR(system->getDevice(), &swapchainInfo, nullptr, &swapchain), "Failed to create swapchain.\n");

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(system->getDevice(), swapchain, &imageCount, nullptr);
    images.create(imageCount);
    views.create(imageCount);
    vkGetSwapchainImagesKHR(system->getDevice(), swapchain, &imageCount, images.getPtr());

    VkComponentMapping components;
    components = 
    {
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY
    };
    VkImageSubresourceRange subresource;
    subresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1
    };

    for(auto ind = 0; ind < imageCount; ++ind)
    {
        VkImageViewCreateInfo viewInfo;
        viewInfo = 
        {
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            nullptr,
            0,
            images[ind],
            VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
            formats[0].format,
            components,
            subresource
        };
        checkResult(vkCreateImageView(system->getDevice(), &viewInfo, nullptr, &views[ind]), "Failed to create image view.\n");
    }
}

const VkImageView& Swapchain::getView(const uint32_t index) const
{
    return views[index];
}

const VkImage& Swapchain::getImage(const uint32_t index) const
{
    return images[index];
}

void Swapchain::destroy()
{
    for(auto ind = 0; ind < views.getSize(); ++ind)
    {
        if(views[ind])
        {
            vkDestroyImageView(system->getDevice(), views[ind], nullptr);
            views[ind] = 0;
        }
    }
    if(swapchain)
    {
        vkDestroySwapchainKHR(system->getDevice(), swapchain, nullptr);
        swapchain = 0;
    }
}