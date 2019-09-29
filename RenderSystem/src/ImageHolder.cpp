#include<ImageHolder.hpp>
#include<cmath>

void ImageHolder::recordLayoutChangeCommands(const VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkImageSubresourceRange& subresource)
{
    if(oldLayout == newLayout) return;
    VkAccessFlags srcAccessFlags = 0, dstAccessFlags = 0;
    VkPipelineStageFlags srcStageFlags = 0, dstStageFlags = 0;
    if(oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED)
    {
        srcStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }
        else if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        else if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        }
        else if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        }
        else reportError("Unsupported layout transition.\n");
    }
    else if(oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        srcStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        srcAccessFlags = VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
        if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        }
        else reportError("Unsupported layout transition.\n");
    }
    else if(oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        srcStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
        srcAccessFlags = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        }
        else if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_TRANSFER_READ_BIT;
        }
        else reportError("Unsupported layout transition.\n");
    }
    else if(oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        srcStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
        srcAccessFlags = VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT;
        if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        else reportError("Unsupported layout transition.\n");
    }
    else if(oldLayout == VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        srcStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        srcAccessFlags = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        if(newLayout == VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        {
            dstStageFlags = VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dstAccessFlags = VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }
    }
    else reportError("Unsupported layout transition.\n");

    VkImageMemoryBarrier imageBarrier = 
    {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        nullptr,
        srcAccessFlags,
        dstAccessFlags,
        oldLayout,
        newLayout,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        img,
        subresource
    };

    vkCmdPipelineBarrier(cmd, srcStageFlags, dstStageFlags, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);
}

void ImageHolder::recordMipmapGenCommands(const VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkExtent2D& imageExtent, const uint32_t mipmapLevelCount, const VkImageLayout mipmapImageLayout)
{
    if(mipmapLevelCount == 1) return;
    VkImageSubresourceRange srcSubresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
        0,
        1,
        0,
        1
    }, 
    dstSubresource = 
    {
        VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT,
        1,
        1,
        0,
        1
    };

    if(oldLayout != VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        recordLayoutChangeCommands(cmd, oldLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img, srcSubresource);
    }
    VkExtent2D srcExtent = imageExtent;
    for(auto i = 1; i < mipmapLevelCount; ++i)
    {
        srcSubresource.baseMipLevel = i - 1;
        dstSubresource.baseMipLevel = i;
        VkImageSubresourceLayers srcLayers = 
        {
            srcSubresource.aspectMask,
            srcSubresource.baseMipLevel,
            srcSubresource.baseArrayLayer,
            srcSubresource.layerCount
        }, 
        dstLayers = 
        {
            dstSubresource.aspectMask,
            dstSubresource.baseMipLevel,
            dstSubresource.baseArrayLayer,
            dstSubresource.layerCount
        };
        recordLayoutChangeCommands(cmd, mipmapImageLayout, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, img, dstSubresource);
        VkImageBlit blitInfo = 
        {
            srcLayers,
            {{0, 0, 0}, {static_cast<int32_t>(srcExtent.width), static_cast<int32_t>(srcExtent.height), 1}},
            dstLayers,
            {{0, 0, 0}, {static_cast<int32_t>(srcExtent.width / 2), static_cast<int32_t>(srcExtent.height) / 2, 1}},
        };
        vkCmdBlitImage(cmd, img, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitInfo, VkFilter::VK_FILTER_LINEAR);
        srcExtent.width /= 2;
        srcExtent.height /= 2;
        recordLayoutChangeCommands(cmd, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, img, dstSubresource);
    }
    srcSubresource.baseMipLevel = 0;
    srcSubresource.levelCount = mipmapLevelCount;
    recordLayoutChangeCommands(cmd, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, newLayout, img, srcSubresource);
}

const uint32_t ImageHolder::getMipmapLevelCount(const VkExtent2D& imageExtent)
{
    return std::log2(std::min(imageExtent.width, imageExtent.height));
}

const uint32_t ImageHolder::getMipmapLevelCount(const VkExtent3D& imageExtent)
{
    return std::log2(std::min(imageExtent.width, imageExtent.height));
}

bool ImageHolder::checkFormatSupport(const VkFormat format, const VkFormatFeatureFlags features, const VkImageTiling tiling) const
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

ImageHolder::ImageHolder(){}

void ImageHolder::create(const System* system)
{
    this->system = system;
}

const size_t ImageHolder::getCurrentImageCount() const
{
    return images.size();
}

const size_t ImageHolder::getCurrentViewCount() const
{
    return views.size();
}

const size_t ImageHolder::getCurrentSamplerCount() const
{
    return samplers.size();
}

const VkImage& ImageHolder::getImage(const uint32_t index) const
{
    return images[index];
}

const VkImageView& ImageHolder::getView(const uint32_t index) const
{
    return views[index];
}

const VkSampler& ImageHolder::getSampler(const uint32_t index) const
{
    return samplers[index];
}

void ImageHolder::addImages(const uint32_t count)
{
    images.insert(images.end(), count, VkImage());
}

void ImageHolder::addViews(const uint32_t count)
{
    views.insert(views.end(), count, VkImageView());
}

void ImageHolder::addSamplers(const uint32_t count)
{
    samplers.insert(samplers.end(), count, VkSampler());
}

void ImageHolder::initImage(const uint32_t index, 
    const VkImageCreateFlags flags,
    const VkImageType type, 
    const VkFormat format, 
    const VkExtent3D& extent, 
    const bool enableMipmapping,
    uint32_t& mipmapLevels,                 // if mipmapping is enabled, rewrites with mipmap level count
    const VkSampleCountFlagBits samples,
    const VkImageTiling tiling, 
    const VkImageUsageFlags usage)
{
    VkImageFormatProperties formatProperties;
    vkGetPhysicalDeviceImageFormatProperties(system->getPhysicalDevice(), format, type, tiling, usage, flags, &formatProperties);
    if((samples & formatProperties.sampleCounts) == 0) reportError("Not supported sample count.\n");
    if(extent.width * extent.height * extent.depth > formatProperties.maxResourceSize) reportError("Too large image.\n");
    if(enableMipmapping)
    {
        mipmapLevels = std::min(formatProperties.maxMipLevels, getMipmapLevelCount(extent));
    }
    VkImageCreateInfo imageInfo = 
    {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        flags,
        type,
        format,
        extent,
        enableMipmapping ? mipmapLevels : 1,
        1,
        samples,
        tiling,
        usage,
        VkSharingMode::VK_SHARING_MODE_EXCLUSIVE,
        1,
        &(system->getGraphicsQueue().familyIndex),
        VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED
    };
    checkResult(vkCreateImage(system->getDevice(), &imageInfo, nullptr, &images[index]), "Failed to create image.\n");
}

void ImageHolder::initView(const uint32_t index, const uint32_t imageIndex, const VkImageViewType type, const VkFormat format, const VkImageSubresourceRange& subresource)
{
    VkComponentMapping mapping = 
    {
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY,
        VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY
    };
    VkImageViewCreateInfo viewInfo = 
    {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        nullptr,
        0,
        images[imageIndex],
        type,
        format,
        mapping,
        subresource
    };
    checkResult(vkCreateImageView(system->getDevice(), &viewInfo, nullptr, &views[index]), "Failed to create view.\n");
}

void ImageHolder::initSampler(const uint32_t index, const float minLod, const float maxLod)
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
        minLod,
        maxLod,
        VkBorderColor::VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
        VK_FALSE
    };
    checkResult(vkCreateSampler(system->getDevice(), &samplerInfo, nullptr, &samplers[index]), "Failed to create sampler.\n");
}

void ImageHolder::destroyImage(const uint32_t index)
{
    if(images[index])
    {
        vkDestroyImage(system->getDevice(), images[index], nullptr);
        images[index] = 0;
    }
}

void ImageHolder::destroyView(const uint32_t index)
{
    if(views[index])
    {
        vkDestroyImageView(system->getDevice(), views[index], nullptr);
        views[index] = 0;
    }
}

void ImageHolder::destroySampler(const uint32_t index)
{
    if(samplers[index])
    {
        vkDestroySampler(system->getDevice(), samplers[index], nullptr);
        samplers[index] = 0;
    }
}

const VkMemoryRequirements ImageHolder::getMemoryRequirements(const uint32_t index) const
{
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(system->getDevice(), images[index], &requirements);
    return requirements;
}

void ImageHolder::bindMemory(const VkDeviceMemory& memory, const uint32_t offset, const uint32_t imageIndex) const
{
    checkResult(vkBindImageMemory(system->getDevice(), images[imageIndex], memory, offset), "Failed to bind image memory.\n");
}

void ImageHolder::destroy()
{
    for(auto index = 0; index < images.size(); ++index)
    {
        destroyImage(index);
    }
    for(auto index = 0; index < views.size(); ++index) destroyView(index);
    for(auto index = 0; index < samplers.size(); ++index) destroySampler(index);
    images.clear();
    views.clear();
    samplers.clear();
}

ImageHolder::~ImageHolder()
{
    destroy();
}