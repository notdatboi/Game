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
        const vk::ImageView& getView() const;
        HardwareImageBuffer image;
        vk::ImageView view;
    };
} 

#endif