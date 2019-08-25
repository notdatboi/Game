#include<Texture.hpp>

namespace spk
{
    void Texture::setMipmapLevelCount(const uint32_t levelCount)
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
        image.setAccessibility(HardwareResourceAccessibility::Static);
        image.setShadowBufferPolicy(false);
        image.setAspect(vk::ImageAspectFlagBits::eColor);
        usage = /*vk::ImageUsageFlagBits::eColorAttachment | */vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
    }

    void Texture::setFormat(const vk::Format format)
    {
        vk::FormatFeatureFlags neededFormatFeatures = vk::FormatFeatureFlagBits::eTransferDst | vk::FormatFeatureFlagBits::eSampledImage /*| vk::FormatFeatureFlagBits::eColorAttachment*/;
        if(image.getMipmapLevelCount() > 1)
        {
            neededFormatFeatures |= vk::FormatFeatureFlagBits::eBlitSrc;
            neededFormatFeatures |= vk::FormatFeatureFlagBits::eBlitDst;
        }
        if(HardwareImageBuffer::getSupportedFormat({format}, vk::ImageTiling::eOptimal, neededFormatFeatures).has_value()) image.setFormat(format);
        else throw std::invalid_argument("Invalid or not supported image format.\n");
    }

    void Texture::setExtent(const vk::Extent3D extent)
    {
        image.setExtent(extent);
    }

    void Texture::load()
    {
        image.setUsage(usage);
        image.load();

        if(!view) createView();
        if(!sampler) createSampler();

        generateMipmaps();
    }

    void Texture::loadFromImage(const Image& src)
    {
        if(image.isLoaded())
        {
            vk::ImageSubresourceRange subresource;
            subresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                .setBaseArrayLayer(0)
                .setLayerCount(1)
                .setBaseMipLevel(0)
                .setLevelCount(1);
            image.changeLayout(vk::ImageLayout::eTransferDstOptimal, subresource);
        }
        else
        {
            image.setUsage(usage);
        }
        
        image.loadFromVkBuffer(src.getData());
        
        if(!view) createView();
        if(!sampler) createSampler();

        generateMipmaps();
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
        image.changeLayout(vk::ImageLayout::eTransferSrcOptimal, subresource);

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
            
            image.changeLayout(vk::ImageLayout::eTransferDstOptimal, currentMipSubresource);
            image.blit(image.getVkImage(), vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eTransferDstOptimal, imageBlit);
            image.changeLayout(vk::ImageLayout::eTransferSrcOptimal, currentMipSubresource);
        }
        subresource.setLevelCount(image.getMipmapLevelCount());
        image.changeLayout(vk::ImageLayout::eShaderReadOnlyOptimal, subresource);
    }

    void Texture::createView()
    {
        view = image.produceImageView();
    }

    void Texture::createSampler()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();

        vk::SamplerCreateInfo samplerInfo;
        samplerInfo.setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eClampToBorder)
            .setAddressModeV(vk::SamplerAddressMode::eClampToBorder)
            .setAddressModeW(vk::SamplerAddressMode::eClampToBorder)
            .setMipLodBias(0)
            .setAnisotropyEnable(false)
            .setMaxAnisotropy(0)
            .setCompareEnable(false)
            .setCompareOp(vk::CompareOp::eAlways)
            .setMinLod(0)
            .setMaxLod(image.getMipmapLevelCount() - 1)
            .setBorderColor(vk::BorderColor::eFloatOpaqueWhite)
            .setUnnormalizedCoordinates(false);
        
        if(logicalDevice.createSampler(&samplerInfo, nullptr, &sampler) != vk::Result::eSuccess) throw std::runtime_error("Failed to create image sampler!\n");
    }

    const vk::Image& Texture::getImage() const
    {
        return image.getVkImage();
    }

    const vk::ImageView& Texture::getView() const
    {
        return view;
    }

    const vk::Sampler& Texture::getSampler() const
    {
        return sampler;
    }

    void Texture::waitUntilReady() const
    {
        image.waitUntilReady();
    }
    
    void Texture::clearResources()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(view)
        {
            logicalDevice.destroyImageView(view, nullptr);
            view = vk::ImageView();
        }
        if(sampler)
        {
            logicalDevice.destroySampler(sampler, nullptr);
            sampler = vk::Sampler();
        }
        image.clearResources();
    }

    Texture::~Texture()
    {
        clearResources();
    }
}