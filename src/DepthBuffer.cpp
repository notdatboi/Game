#include<DepthBuffer.hpp>

namespace spk
{
    DepthBuffer::DepthBuffer()
    {
        image.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment);
        image.setAccessibility(HardwareResourceAccessibility::Static);
        image.setMipmapLevelCount(1);
        image.setShadowBufferPolicy(false);
        image.setAspect(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
    }

    void DepthBuffer::setFormat(const vk::Format format)
    {
        vk::FormatFeatureFlags neededFormatFeatures = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
        if(HardwareImageBuffer::getSupportedFormat({format}, vk::ImageTiling::eOptimal, neededFormatFeatures).has_value()) image.setFormat(format);
        else throw std::invalid_argument("Invalid or not supported image format.\n");
        image.setFormat(format);
    }

    void DepthBuffer::setExtent(const vk::Extent2D extent)
    {
        image.setExtent({extent.width, extent.height, 1});
    }

    void DepthBuffer::load()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        image.load();
        vk::ImageSubresourceRange subresource;
        subresource.setAspectMask(vk::ImageAspectFlagBits::eDepth)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(1);
        image.changeLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal, subresource);

        view = image.produceImageView(0, 1);
    }

    void DepthBuffer::waitUntilReady() const
    {
        image.waitUntilReady();
    }

    void DepthBuffer::cleanResources()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        image.clearResources();
        if(view)
        {
            logicalDevice.destroyImageView(view, nullptr);
            view = vk::ImageView();
        }
    }

    const vk::AttachmentDescription DepthBuffer::getAttachmentDescription(const bool loadDepthContent, const bool saveDepthContent, const bool loadStencilContent, const bool saveStencilContent) const
    {
        vk::AttachmentDescription description;
        description.setFormat(image.getFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(loadDepthContent ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eDontCare)
            .setStoreOp(saveDepthContent ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(loadStencilContent ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(saveStencilContent ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }

    const vk::ImageView& DepthBuffer::getView() const
    {
        return view;
    }

    DepthBuffer::~DepthBuffer()
    {
        cleanResources();
    }
}