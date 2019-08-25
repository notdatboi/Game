#ifndef RENDER_TARGET_HPP
#define RENDER_TARGET_HPP

#include<System.hpp>

namespace spk
{
    class RenderTarget
    {
    public:
        virtual ~RenderTarget();
    protected:
        friend class RenderSystem;
        virtual const vk::AttachmentDescription getAttachmentDescription(const bool loadPreviousContent, const bool saveContent) const = 0;
        virtual const std::vector<vk::ImageView> getViews() const;
    };
}

#endif