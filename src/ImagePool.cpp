#include<ImagePool.hpp>
#include<cmath>

void ImagePool::recordLayouChangeCommands(VkCommandBuffer& cmd, const VkImageLayout oldLayout, const VkImageLayout newLayout, const VkImage& img, const VkImageSubresourceRange& subresource)
{
    VkAccessFlags srcAccessFlags, dstAccessFlags;
    VkPipelineStageFlags srcStageFlags, dstStageFlags;
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

const uint32_t ImagePool::getMipmapLevelCount(const VkExtent2D& imageExtent)
{
    return std::log2(std::min(imageExtent.width, imageExtent.height));
}

bool ImagePool::checkFormatSupport(const VkFormat format, const VkFormatFeatureFlags features, const VkImageTiling tiling) const
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

void ImagePool::createImage(const VkFormat format, const VkExtent3D& extent, const uint32_t mipmapLevels, const VkImageUsageFlags usage, const VkImageAspectFlags aspect, const bool createView, const bool createSampler, const uint32_t index)
{
    VkImageCreateInfo imageInfo = 
    {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        nullptr,
        0,
        VkImageType::VK_IMAGE_TYPE_2D,
        format,
        extent,
        mipmapLevels,
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
            mipmapLevels,
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
            mipmapLevels - 1,
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
    images.clean();
}

ImagePool::~ImagePool()
{
    destroy();
}