#include"../include/Swapchain.hpp"

namespace spk
{
    Swapchain::Swapchain(){}

    Swapchain::Swapchain(const vk::SurfaceKHR& surface,
        const uint32_t minImageCount,
        vk::Format& preferredFormat,
        const vk::Extent2D extent,
        const vk::ImageUsageFlags usageFlags,
        const vk::ImageAspectFlags aspectFlags,
        const vk::PresentModeKHR presentMode,
        const bool clipped,
        const vk::SwapchainKHR& oldSwapchain)
    {
        create(surface, minImageCount, preferredFormat, extent, usageFlags, aspectFlags, presentMode, clipped, oldSwapchain);
    }

    void Swapchain::create(const vk::SurfaceKHR& surface,
        const uint32_t minImageCount,
        vk::Format& preferredFormat,
        const vk::Extent2D extent,
        const vk::ImageUsageFlags usageFlags,
        const vk::ImageAspectFlags aspectFlags,
        const vk::PresentModeKHR presentMode,
        const bool clipped,
        const vk::SwapchainKHR& oldSwapchain)
    {
        const vk::PhysicalDevice& physicalDevice = system::System::getInstance()->getPhysicalDevice();
        const vk::Device logicalDevice = system::System::getInstance()->getLogicalDevice();

        vk::SurfaceCapabilitiesKHR capabilities;
        if(physicalDevice.getSurfaceCapabilitiesKHR(surface, &capabilities) != vk::Result::eSuccess) throw std::runtime_error("Failed to get surface capabilities!\n");
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        uint32_t formatCount;
        physicalDevice.getSurfaceFormatsKHR(surface, &formatCount, nullptr);
        surfaceFormats.resize(formatCount);
        physicalDevice.getSurfaceFormatsKHR(surface, &formatCount, surfaceFormats.data());
        
        uint32_t imageCount;
        if(minImageCount >= capabilities.minImageCount && minImageCount <= capabilities.maxImageCount) imageCount = minImageCount;
        else if(minImageCount < capabilities.minImageCount) imageCount = capabilities.minImageCount;
        else if(minImageCount > capabilities.maxImageCount) imageCount = capabilities.maxImageCount;

        bool chosenPreferredFormat = false;
        vk::SurfaceFormatKHR chosenFormat;
        for(const auto& format : surfaceFormats)
        {
            if(format.format == preferredFormat)
            {
                chosenPreferredFormat = true;
                chosenFormat = format;
                break;
            }
        }
        if(!chosenPreferredFormat)
        {
            chosenFormat = surfaceFormats[0];
            preferredFormat = chosenFormat.format;
#ifdef DEBUG
            std::cout << "No way to choose preferred swapchain format.\n";
#endif
        }

        if((capabilities.supportedUsageFlags & usageFlags) != usageFlags) throw std::runtime_error("No way to enable all the requested usage flags.\n");

        vk::SharingMode sharingMode;
        std::vector<uint32_t> queueFamilyIndices;
        uint32_t graphicsQueueFamIndex = system::Executives::getInstance()->getGraphicsQueueFamilyIndex(), presentQueueFamIndex = system::Executives::getInstance()->getPresentQueue(surface).first;

        if(graphicsQueueFamIndex == presentQueueFamIndex)
        {
            sharingMode = vk::SharingMode::eExclusive;
            queueFamilyIndices = {graphicsQueueFamIndex};
        }
        else
        {
            sharingMode = vk::SharingMode::eConcurrent;
            queueFamilyIndices = {graphicsQueueFamIndex, presentQueueFamIndex};
        }

        vk::SwapchainCreateInfoKHR info;
        info.setSurface(surface)
            .setMinImageCount(imageCount)
            .setImageFormat(chosenFormat.format)
            .setImageColorSpace(chosenFormat.colorSpace)
            .setImageExtent(extent)
            .setImageArrayLayers(1)
            .setImageUsage(usageFlags)
            .setImageSharingMode(sharingMode)
            .setQueueFamilyIndexCount(queueFamilyIndices.size())
            .setPQueueFamilyIndices(queueFamilyIndices.data())
            .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(presentMode)
            .setClipped(clipped)
            .setOldSwapchain(oldSwapchain);
        
        if(logicalDevice.createSwapchainKHR(&info, nullptr, &swapchain) != vk::Result::eSuccess) throw std::runtime_error("Failed to crate swapchain!\n");

        logicalDevice.getSwapchainImagesKHR(swapchain, &imageCount, nullptr);
        std::vector<vk::Image> tmpImages(imageCount);
        swapchainImages.resize(imageCount);
        swapchainImageViews.resize(imageCount);
        logicalDevice.getSwapchainImagesKHR(swapchain, &imageCount, tmpImages.data());

        for(auto index = 0; index < imageCount; ++index)
        {
            swapchainImages[index].setAspect(aspectFlags);
            swapchainImages[index].setAccessibility(HardwareResourceAccessibility::Static);
            swapchainImages[index].setExtent({extent.width, extent.height, 1});
            swapchainImages[index].setFormat(chosenFormat.format);
            swapchainImages[index].setMipmapLevelCount(1);
            swapchainImages[index].setShadowBufferPolicy();
            swapchainImages[index].setUsage(usageFlags);
            swapchainImages[index].takeOwnership(tmpImages[index]);
            swapchainImageViews[index] = swapchainImages[index].produceImageView();
        }
    }

    const uint32_t Swapchain::acquireNextImageIndex(const vk::Semaphore& signalSemaphore, const vk::Fence& signalFence) const
    {
        const vk::Device logicalDevice = system::System::getInstance()->getLogicalDevice();

        uint32_t index;
        if(logicalDevice.acquireNextImageKHR(swapchain, ~0U, signalSemaphore, signalFence, &index) != vk::Result::eSuccess) throw std::runtime_error("Failed to acquire image.\n");

        return index;
    }

    const std::vector<vk::ImageView>& Swapchain::getImageViews() const
    {
        return swapchainImageViews;
    }

    const vk::SwapchainKHR& Swapchain::getSwapchain() const
    {
        return swapchain;
    }

    HardwareImageBuffer& Swapchain::getImage(const uint32_t index)
    {
        return swapchainImages[index];
    }

    void Swapchain::destroy()
    {
        const vk::Device logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(swapchainImageViews.size() != 0)
        {
            for(auto& view : swapchainImageViews)
            {
                if(view)
                {
                    logicalDevice.destroyImageView(view, nullptr);
                    view = vk::ImageView();
                }
            }
            for(auto& image : swapchainImages) image.releaseImage();
        }
        if(swapchain)
        {
            logicalDevice.destroySwapchainKHR(swapchain, nullptr);
        }
    }

    Swapchain::~Swapchain()
    {
        destroy();
    }
}