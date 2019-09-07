#include<Swapchain.hpp>

Swapchain::Swapchain()
{

}

void Swapchain::create(const System* system, uint32_t& imageCount, const VkFormat preferredFormat)
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
    VkSurfaceFormatKHR chosenFormat;
    if(preferredFormat != VkFormat::VK_FORMAT_UNDEFINED)
    {
        for(auto ind = 0; ind < formats.getSize(); ++ind)
        {
            if(formats[ind].format == format)
            {
                chosenFormat = formats[ind];
                break;
            }
        }
    }
    else
    {
        chosenFormat = formats[0];
    }
    format = chosenFormat.format;
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    if(imageCount > surfaceCapabilities.maxImageCount) imageCount = surfaceCapabilities.maxImageCount;
    if(imageCount < surfaceCapabilities.minImageCount) imageCount = surfaceCapabilities.minImageCount;
    VkImageUsageFlags neededUsageFlags = VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if((surfaceCapabilities.supportedUsageFlags & neededUsageFlags) != neededUsageFlags) reportError("Invalid surface.\n");

    extent = surfaceCapabilities.currentExtent;
    VkSwapchainCreateInfoKHR swapchainInfo;
    swapchainInfo = 
    {
        VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        nullptr,
        0,
        surface,
        imageCount,
        chosenFormat.format,
        chosenFormat.colorSpace,
        extent,
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

    imageCount = 0;
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
            chosenFormat.format,
            components,
            subresource
        };
        checkResult(vkCreateImageView(system->getDevice(), &viewInfo, nullptr, &views[ind]), "Failed to create image view.\n");
    }
}

const VkFormat& Swapchain::getFormat() const
{
    return format;
}

const VkExtent2D& Swapchain::getExtent() const
{
    return extent;
}

const uint32_t Swapchain::acquireNextImage(const VkSemaphore& signalSemaphore, const VkFence& signalFence) const
{
    uint32_t index;
    checkResult(vkAcquireNextImageKHR(system->getDevice(), swapchain, ~0, signalSemaphore, signalFence, &index), "Failed to acquire image.\n");
    return index;
}

const uint32_t Swapchain::getImageCount() const
{
    return images.getSize();
}

const VkSwapchainKHR& Swapchain::getSwapchain() const
{
    return swapchain;
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
    images.clean();
    views.clean();
}

Swapchain::~Swapchain()
{
    destroy();
}