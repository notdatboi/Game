#ifndef SYNCHRONIZATION_POOL
#define SYNCHRONIZATION_POOL
#include<System.hpp>
#include<Utils.hpp>

class SynchronizationPool
{
public:
    SynchronizationPool();
    void create(const System* system, const uint32_t fenceCount, const uint32_t semaphoreCount);
    void createFence(const uint32_t index, const bool signaled);
    void createSemaphore(const uint32_t index);
    const VkFence& getFence(const uint32_t index) const;
    const VkSemaphore& getSemaphore(const uint32_t index) const;
    void resetFences(const uint32_t first, const uint32_t count = 1);
    void destroy();
    ~SynchronizationPool();
private:
    const System* system;
    Array<VkFence> fences;
    Array<VkSemaphore> semaphores;
};

#endif