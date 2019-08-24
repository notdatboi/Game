#ifndef SPARK_DEPTH_BUFFER_HPP
#define SPARK_DEPTH_BUFFER_HPP

#include<HardwareImageBuffer.hpp>

namespace spk
{
    class DepthBuffer
    {
    public:
        DepthBuffer();
        void setFormat(const vk::Format format);
        void setExtent(const vk::Extent2D extent);
        void load();
        void waitUntilReady() const;
        void cleanResources();
        ~DepthBuffer();
    private:
        friend class RenderSystem;
        const vk::AttachmentDescription getAttachmentDescription(const bool loadDepthContent, const bool saveDepthContent, const bool loadStencilContent, const bool saveStencilContent) const;
        const vk::ImageView& getView() const;
        HardwareImageBuffer image;
        vk::ImageView view;
    };
} 

#endif