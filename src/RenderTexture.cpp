#include<RenderTexture.hpp>

namespace spk
{
    RenderTexture::RenderTexture()
    {
        image.setAccessibility(HardwareResourceAccessibility::Static);
        image.setMipmapLevelCount(1);
        image.setShadowBufferPolicy(false);
        usage = vk::ImageUsageFlagBits::eColorAttachment;
    }

    void RenderTexture::useAsInput()
    {
        usage |= vk::ImageUsageFlagBits::eInputAttachment;
    }

    void RenderTexture::setFormat(const vk::Format format)
    {
        vk::FormatFeatureFlags neededFormatFeatures = vk::FormatFeatureFlagBits::eColorAttachment;
        if(HardwareImageBuffer::getSupportedFormat({format}, vk::ImageTiling::eOptimal, neededFormatFeatures).has_value()) image.setFormat(format);
        else throw std::invalid_argument("Invalid or not supported image format.\n");
        image.setFormat(format);
    }

    void RenderTexture::setExtent(const vk::Extent2D extent)
    {
        image.setExtent({extent.width, extent.height, 1});
    }

    void RenderTexture::load()
    {
        image.setUsage(usage);
        image.load();

        vk::ImageSubresourceRange subresource;
        subresource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setBaseMipLevel(0)
            .setLevelCount(1);

        createView();        

        image.changeLayout(vk::ImageLayout::eColorAttachmentOptimal, subresource);
    }

    void RenderTexture::createView()
    {
        view = image.produceImageView();
    }

    void RenderTexture::waitUntilReady() const
    {
        image.waitUntilReady();
    }

    void RenderTexture::clearResources()
    {
        const vk::Device& logicalDevice = system::System::getInstance()->getLogicalDevice();

        image.clearResources();
        if(view)
        {
            logicalDevice.destroyImageView(view, nullptr);
            view = vk::ImageView();
        }
    }

    RenderTexture::~RenderTexture()
    {
        clearResources();
    }

    const vk::AttachmentDescription RenderTexture::getAttachmentDescription(const bool loadPreviousContent, const bool saveContent) const
    {
        vk::AttachmentDescription description;
        description.setFormat(image.getFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(loadPreviousContent ? vk::AttachmentLoadOp::eLoad : vk::AttachmentLoadOp::eClear)
            .setStoreOp(saveContent ? vk::AttachmentStoreOp::eStore : vk::AttachmentStoreOp::eDontCare)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
        return description;
    }

    const std::vector<vk::ImageView> RenderTexture::getViews() const
    {
        return {view};
    }
}