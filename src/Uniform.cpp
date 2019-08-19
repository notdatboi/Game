#include<Uniform.hpp>

namespace spk
{
    Uniform::Uniform()
    {
        buffer.setUsage(vk::BufferUsageFlagBits::eUniformBuffer);
    }

    void Uniform::setShadowBufferPolicy(const bool use)
    {
        buffer.setShadowBufferPolicy(use);
    }

    void Uniform::setAccessibility(const HardwareResourceAccessibility accessibility)
    {
        buffer.setAccessibility(accessibility);
    }

    void Uniform::setSize(const uint32_t size)
    {
        buffer.setSize(size);
    }

    void Uniform::waitUntilReady() const
    {
        buffer.waitUntilReady();
    }

    void Uniform::load()
    {
        buffer.load();
    }

    void Uniform::loadFromBuffer(const HardwareBuffer& src)
    {
        buffer.loadFromBuffer(src);
    }

    void Uniform::loadFromMemory(const void* src)
    {
        buffer.loadFromMemory(src);
    }

    void Uniform::clearResources()
    {
        buffer.clearResources();
    }

    const uint32_t Uniform::getSize() const
    {
        return buffer.getSize();
    }

    const vk::Buffer& Uniform::getVkBuffer() const
    {
        return buffer.getVkBuffer();
    }

    Uniform::~Uniform()
    {
        clearResources();
    }
}