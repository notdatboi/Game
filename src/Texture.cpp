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

        return *this;
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

        return *this;
    }

    Texture& Texture::loadFromImage(const Image& src)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};
        image.setExtent(src.getExtent())
            .setUsage(usage)
            .loadFromVkBuffer(src.getData(), vk::ImageAspectFlagBits::eColor);
        
        generateMipmaps();
        createView();

        return *this;
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

    void Texture::createView()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();

        vk::ImageSubresourceRange subresource;
        subresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(image.getMipmapLevelCount());

        vk::ComponentMapping components;
        components.setR(vk::ComponentSwizzle::eR)
            .setG(vk::ComponentSwizzle::eG)
            .setB(vk::ComponentSwizzle::eB)
            .setA(vk::ComponentSwizzle::eA);

        vk::ImageViewCreateInfo viewInfo;
        viewInfo.setImage(image.getVkImage())
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(image.getFormat())
            .setComponents(components)
            .setSubresourceRange(subresource);

        if(logicalDevice.createImageView(&viewInfo, nullptr, &view) != vk::Result::eSuccess) throw std::runtime_error("Failed to create image view!\n");
    }
    
    void Texture::clearResources()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(view)
        {
            logicalDevice.destroyImageView(view, nullptr);
            view = vk::ImageView();
        }
        image.clearResources();
    }

    Texture::~Texture()
    {
        clearResources();
    }
}