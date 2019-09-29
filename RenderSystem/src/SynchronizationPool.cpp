#include<SynchronizationPool.hpp>

SynchronizationPool::SynchronizationPool(){}

void SynchronizationPool::create(const System* system)
{
    this->system = system;
}

void SynchronizationPool::addFences(const uint32_t count, const bool signaled)
{
    VkFenceCreateInfo info = 
    {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        nullptr,
        signaled ? VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT : VkFenceCreateFlags()
    };
    auto first = getFenceCount();
    fences.insert(fences.end(), count, VkFence());
    for(auto ind = first; ind < fences.size(); ++ind)
    {
        checkResult(vkCreateFence(system->getDevice(), &info, nullptr, &fences[ind]), "Failed to create fence.\n");
    }
}

const size_t SynchronizationPool::getFenceCount() const
{
    return fences.size();
}

void SynchronizationPool::addSemaphores(const uint32_t count)
{
    VkSemaphoreCreateInfo info = 
    {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        nullptr,
        0
    };
    auto first = getSemaphoreCount();
    semaphores.insert(semaphores.end(), count, VkSemaphore());
    for(auto ind = first; ind < semaphores.size(); ++ind)
    {
        checkResult(vkCreateSemaphore(system->getDevice(), &info, nullptr, &semaphores[ind]), "Failed to create semaphore.\n");
    }
}

const size_t SynchronizationPool::getSemaphoreCount() const
{
    return semaphores.size();
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

void SynchronizationPool::resetFences(const uint32_t first, const uint32_t count) const
{
    checkResult(vkResetFences(system->getDevice(), count, &fences[first]), "Failed to reset fences.\n");
}

void SynchronizationPool::destroy()
{
    for(auto ind = 0; ind < fences.size(); ++ind)
    {
        if(fences[ind])
        {
            vkDestroyFence(system->getDevice(), fences[ind], nullptr);
            fences[ind] = 0;
        }
    }
    for(auto ind = 0; ind < semaphores.size(); ++ind)
    {
        if(semaphores[ind])
        {
            vkDestroySemaphore(system->getDevice(), semaphores[ind], nullptr);
            semaphores[ind] = 0;
        }
    }
    fences.clear();
    semaphores.clear();
}

SynchronizationPool::~SynchronizationPool()
{
    destroy();
}