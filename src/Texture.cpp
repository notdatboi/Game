#include<Texture.hpp>

namespace spk
{
    Texture& Texture::setMipmapLevelCount(const uint32_t levelCount)
    {
        image.setMipmapLevelCount(levelCount);
        if(levelCount > 1)
        {
            usage |= vk::ImageUsageFlagBits::eTransferSrc;
        }
    }

    Texture::Texture()
    {
        setMipmapLevelCount(1);
        try
        {
            setFormat(vk::Format::eR8G8B8A8Unorm);
        }
        catch(std::runtime_error err){}
        usage = /*vk::ImageUsageFlagBits::eColorAttachment | */vk::ImageUsageFlagBits::eTransferDst;
    }

    Texture& Texture::setFormat(const vk::Format format)
    {
        vk::FormatFeatureFlags neededFormatFeatures = vk::FormatFeatureFlagBits::eTransferDst /*| vk::FormatFeatureFlagBits::eColorAttachment*/;
        if(image.getMipmapLevelCount() > 1)
        {
            neededFormatFeatures |= vk::FormatFeatureFlagBits::eBlitSrc;
            neededFormatFeatures |= vk::FormatFeatureFlagBits::eBlitDst;
        }
        if(HardwareImageBuffer::getSupportedFormat({format}, vk::ImageTiling::eOptimal, neededFormatFeatures).has_value()) image.setFormat(format);
        else throw std::invalid_argument("Invalid or not supported image format.\n");
    }

    Texture& Texture::loadFromImage(const Image& src)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};
        image.setExtent(src.getExtent())
            .setUsage(usage)
            .loadFromVkBuffer(src.getData(), vk::ImageAspectFlagBits::eColor);
    }

    void Texture::generateMipmaps()
    {
        if(image.getMipmapLevelCount() <= 1) return;
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        vk::ImageSubresourceRange subresource;
        subresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(1);
        image.changeLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, subresource);

        for(auto i = 1; i < image.getMipmapLevelCount(); ++i)
        {
            vk::ImageSubresourceLayers srcSubresource, dstSubresource;

            srcSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setMipLevel(i - 1);
            dstSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setMipLevel(i);

            std::array<vk::Offset3D, 2> srcOffsets, dstOffsets;
            srcOffsets[1] = {extent.width >> (i - 1), extent.height >> (i - 1), extent.depth};
            dstOffsets[1] = {extent.width >> i, extent.height >> i, extent.depth};

            vk::ImageBlit imageBlit;
            imageBlit.setSrcSubresource(srcSubresource)
                .setDstSubresource(dstSubresource)
                .setSrcOffsets(srcOffsets)
                .setDstOffsets(dstOffsets);
            
            vk::ImageSubresourceRange currentMipSubresource;
            currentMipSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setBaseMipLevel(i)
                .setLevelCount(1);
            
            image.changeLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, currentMipSubresource)
                .blit(image.getVkImage(), vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eTransferDstOptimal, imageBlit)
                .changeLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, currentMipSubresource);
        }
        subresource.setLevelCount(image.getMipmapLevelCount());
        image.changeLayout(vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, subresource);
    }
}