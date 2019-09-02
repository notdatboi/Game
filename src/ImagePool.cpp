#include<ImagePool.hpp>
#include<cmath>

bool ImagePool::checkFormatSupport(const VkFormat& format, const VkFormatFeatureFlags& features, const VkImageTiling& tiling) const
{
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(system->getPhysicalDevice(), format, &properties);
    if(tiling == VkImageTiling::VK_IMAGE_TILING_LINEAR)
    {
        if((properties.linearTilingFeatures & features) == features) return true;
    }
    else if(tiling == VkImageTiling::VK_IMAGE_TILING_OPTIMAL)
    {
        if((properties.optimalTilingFeatures & features) == features) return true;
    }
    return false;
}

ImagePool::ImagePool(){}

void ImagePool::create(const System* system, const uint32_t count)
{
    this->system = system;
    images.create(count);
}

void ImagePool::createImage(const VkFormat& format, const VkExtent3D& extent, const bool useMipmapping, const VkImageUsageFlags& usage, const VkImageAspectFlags& aspect, const bool createView, const bool createSampler, const uint32_t index)
{
    uint32_t mipLevelCount = useMipmapping ? std::log2(std::min(extent.width, extent.height)) : 1;
    VkImageCreateInfo imageInfo = 
    {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        0,
        VkImageType::VK_IMAGE_TYPE_2D,
        format,
        extent,
        mipLevelCount,
        1,
        VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
        VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
        usage,
        VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
        1,
        &(system->getGraphicsQueue().familyIndex),
        VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED
    };
    checkResult(vkCreateImage(system->getDevice(), &imageInfo, nullptr, &images[index].image), "Failed to create image.\n");

    if(createView)
    {
        VkComponentMapping mapping = 
        {
            VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
            VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
            VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
            VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY
        };
        VkImageSubresourceRange subresource = 
        {
            aspect,
            0,
            mipLevelCount,
            0,
            1
        };
        VkImageViewCreateInfo viewInfo = 
        {
            VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            nullptr,
            0,
            images[index].image,
            VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
            format,
            mapping,
            subresource
        };
        checkResult(vkCreateImageView(system->getDevice(), &viewInfo, nullptr, &images[index].view), "Failed to create view.\n");
    }

    if(createSampler)
    {
        VkSamplerCreateInfo samplerInfo =
        {
            VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            nullptr,
            0,
            VkFilter::VK_FILTER_LINEAR,
            VkFilter::VK_FILTER_LINEAR,
            VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
            VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
            0,
            VK_FALSE,
            0,
            VK_FALSE,
            VkCompareOp::VK_COMPARE_OP_ALWAYS,
            0,
            mipLevelCount - 1,
            VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            VK_FALSE
        };
        checkResult(vkCreateSampler(system->getDevice(), &samplerInfo, nullptr, &images[index].sampler), "Failed to create sampler.\n");
    }
}

const VkMemoryRequirements ImagePool::getMemoryRequirements(const uint32_t index)
{
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(system->getDevice(), images[index].image, &requirements);
    return requirements;
}

void ImagePool::bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t imageIndex)
{
    checkResult(vkBindImageMemory(system->getDevice(), images[imageIndex].image, memory, offset), "Failed to bind image memory.\n");
}

const ImageInfo& ImagePool::operator[](const uint32_t index) const
{
    return images[index];
}

ImageInfo& ImagePool::operator[](const uint32_t index)
{
    return images[index];
}

void ImagePool::destroy()
{
    for(auto index = 0; index < images.getSize(); ++index)
    {
        if(images[index].view)
        {
            vkDestroyImageView(system->getDevice(), images[index].view, nullptr);
            images[index].view = 0;
        }
        if(images[index].sampler)
        {
            vkDestroySampler(system->getDevice(), images[index].sampler, nullptr);
            images[index].sampler = 0;
        }
        if(images[index].image)
        {
            vkDestroyImage(system->getDevice(), images[index].image, nullptr);
            images[index].image = 0;
        }
    }
}
ImagePool::~ImagePool()
{
    destroy();
}