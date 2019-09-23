#ifndef SYNCHRONIZATION_POOL
#define SYNCHRONIZATION_POOL
#include<System.hpp>
#include<vector>

class SynchronizationPool
{
public:
    SynchronizationPool();
    void create(const System* system);
    void addFences(const uint32_t count, const bool signaled);
    const size_t getFenceCount() const;
    void addSemaphores(const uint32_t count);
    const size_t getSemaphoreCount() const;
    const VkFence& getFence(const uint32_t index) const;
    const VkSemaphore& getSemaphore(const uint32_t index) const;
    void waitForFences(const uint32_t first, const uint32_t count = 1, const VkBool32 waitAll = VK_TRUE, const uint64_t timeout = (~0ull)) const;
    void resetFences(const uint32_t first, const uint32_t count = 1) const;
    void destroy();
    ~SynchronizationPool();
private:
    const System* system;
    std::vector<VkFence> fences;
    std::vector<VkSemaphore> semaphores;
};

#endif