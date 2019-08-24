#ifndef SPARK_RENDER_TEXTURE_HPP
#define SPARK_RENDER_TEXTURE_HPP

#include<RenderTarget.hpp>
#include<HardwareImageBuffer.hpp>

namespace spk
{
    class RenderTexture : public RenderTarget
    {
    public:
        RenderTexture();
        void setFormat(const vk::Format format);
        void setExtent(const vk::Extent2D extent);
        void useAsInput();
        void load();
        void waitUntilReady() const;
        void clearResources();
        virtual ~RenderTexture();
    private:
        virtual const vk::AttachmentDescription getAttachmentDescription(const bool loadPreviousContent, const bool saveContent) const;
        virtual const std::vector<vk::ImageView> getViews() const;
        void createView();

        vk::ImageUsageFlags usage;
        HardwareImageBuffer image;
        vk::ImageView view;        
    };
}

#endif