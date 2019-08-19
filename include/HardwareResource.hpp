#ifndef SPARK_HARDWARE_RESOURCE_HPP
#define SPARK_HARDWARE_RESOURCE_HPP
#include<SparkIncludeBase.hpp>

namespace spk
{
    enum class HardwareResourceAccessibility
    {
        Static,                             // Recommended. Assumption that the buffer is changed rarely, changing takes more time, but reading from it is greatly more optimal. Created with 'shadow buffer' to easily retrieve data
        Dynamic                             // Assumption that the buffer is changed frequently, changing takes almost no time, but usage takes more time
    };

    class HardwareResource
    {
    public:
        virtual void setShadowBufferPolicy(bool use = false) = 0;
        virtual void setAccessibility(const HardwareResourceAccessibility accessibility) = 0;
        virtual void load() = 0;
        virtual void waitUntilReady() const = 0;
        virtual void resetWaiter() = 0;
        virtual void clearResources() = 0;
        virtual ~HardwareResource();
    protected:
    };
}

#endif