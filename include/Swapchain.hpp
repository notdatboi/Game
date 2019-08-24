#ifndef SPARK_SWAPCHAIN_HPP
#define SPARK_SWAPCHAIN_HPP

#include<SparkIncludeBase.hpp>
#include<System.hpp>
#include<Executives.hpp>
#include<vector>

namespace spk
{
    class Swapchain
    {
    public:
        Swapchain();
        Swapchain(const vk::SurfaceKHR& surface,
            const uint32_t minImageCount,
            vk::Format& preferredFormat,
            const vk::Extent2D extent,
            const vk::ImageUsageFlags usageFlags,
            const vk::ImageAspectFlags aspectFlags,
            const vk::PresentModeKHR presentMode,
            const bool clipped,
            const vk::SwapchainKHR& oldSwapchain = vk::SwapchainKHR());
        void create(const vk::SurfaceKHR& surface,
            const uint32_t minImageCount,
            vk::Format& preferredFormat,
            const vk::Extent2D extent,
            const vk::ImageUsageFlags usageFlags,
            const vk::ImageAspectFlags aspectFlags,
            const vk::PresentModeKHR presentMode,
            const bool clipped,
            const vk::SwapchainKHR& oldSwapchain = vk::SwapchainKHR());
        const uint32_t acquireNextImageIndex(const vk::Semaphore& signalSemaphore, const vk::Fence& signalFence) const;
        const std::vector<vk::ImageView>& getImageViews() const;
        const vk::SwapchainKHR& getSwapchain() const;
        void destroy();
        ~Swapchain();
    private:
        vk::SwapchainKHR swapchain;
        std::vector<vk::Image> swapchainImages;
        std::vector<vk::ImageView> swapchainImageViews;
    };
}

#endif