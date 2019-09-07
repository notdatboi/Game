#include<SynchronizationPool.hpp>

SynchronizationPool::SynchronizationPool(){}

void SynchronizationPool::create(const System* system, const uint32_t fenceCount, const uint32_t semaphoreCount)
{
    this->system = system;
    semaphores.create(semaphoreCount);
    fences.create(fenceCount);
}

void SynchronizationPool::createFence(const uint32_t index, const bool signaled)
{
    VkFenceCreateInfo info = 
    {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        nullptr,
        signaled ? VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT : 0
    };
    checkResult(vkCreateFence(system->getDevice(), &info, nullptr, &fences[index]), "Failed to create fence.\n");
}

void SynchronizationPool::createSemaphore(const uint32_t index)
{
    VkSemaphoreCreateInfo info = 
    {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0
    };
    checkResult(vkCreateSemaphore(system->getDevice(), &info, nullptr, &semaphores[index]), "Failed to create semaphore.\n");
}

const VkFence& SynchronizationPool::getFence(const uint32_t index) const
{
    return fences[index];
}

const VkSemaphore& SynchronizationPool::getSemaphore(const uint32_t index) const
{
    return semaphores[index];
}

void SynchronizationPool::waitForFences(const uint32_t first, const uint32_t count, const VkBool32 waitAll, const uint64_t timeout) const
{
    checkResult(vkWaitForFences(system->getDevice(), count, &fences[first], waitAll, timeout), "Failed to wait for fences.\n");
}

void SynchronizationPool::resetFences(const uint32_t first, const uint32_t count = 1) const
{
    checkResult(vkResetFences(system->getDevice(), count, &fences[first]), "Failed to reset fences.\n");
}

void SynchronizationPool::destroy()
{
    for(auto ind = 0; ind < fences.getSize(); ++ind)
    {
        if(fences[ind])
        {
            vkDestroyFence(system->getDevice(), fences[ind], nullptr);
            fences[ind] = 0;
        }
    }
    for(auto ind = 0; ind < semaphores.getSize(); ++ind)
    {
        if(semaphores[ind])
        {
            vkDestroySemaphore(system->getDevice(), semaphores[ind], nullptr);
            semaphores[ind] = 0;
        }
    }
    fences.clean();
    semaphores.clean();
}

SynchronizationPool::~SynchronizationPool()
{
    destroy();
}