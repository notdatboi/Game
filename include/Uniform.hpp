#ifndef SPARK_UNIFORM_HPP
#define SPARK_UNIFORM_HPP

#include<HardwareBuffer.hpp>

namespace spk
{
    class Uniform
    {
    public:
        Uniform();
        void setShadowBufferPolicy(const bool use = false);
        void setAccessibility(const HardwareResourceAccessibility accessibility);
        void setSize(const uint32_t size);
        void waitUntilReady() const;
        void load();
        void loadFromBuffer(const HardwareBuffer& src);
        void loadFromMemory(const void* src);
        void clearResources();
        const uint32_t getSize() const;
        ~Uniform();
    private:
        friend class ShaderSet;
        const vk::Buffer& getVkBuffer() const;
        HardwareBuffer buffer;
    };
}

#endif