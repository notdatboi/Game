#include<CommandPool.hpp>

CommandPool::CommandPool(){}

const VkCommandPool& CommandPool::getPool() const
{
    return pool;
}

void CommandPool::create(const System* system, const uint32_t commandBufferCount, const bool dynamicPool = false)
{
    this->system = system;
    commandBuffers.create(commandBufferCount);
    VkCommandPoolCreateInfo poolInfo = 
    {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        nullptr,
        dynamicPool ? (VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) : 0,
        system->getGraphicsQueue().familyIndex
    };
    checkResult(vkCreateCommandPool(system->getDevice(), &poolInfo, nullptr, &pool), "Failed to create command pool.\n");
}

void CommandPool::allocateCommandBuffers(const uint32_t first, const uint32_t count, const VkCommandBufferLevel level)
{
    VkCommandBufferAllocateInfo allocation = 
    {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        nullptr,
        pool,
        level,
        count
    };
    checkResult(vkAllocateCommandBuffers(system->getDevice(), &allocation, &commandBuffers[first]), "Allocation failed.\n");
}

void CommandPool::reset(const uint32_t index, const bool releaseResources) const
{
    vkResetCommandBuffer(commandBuffers[index], releaseResources ? VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : VkCommandBufferResetFlags());
}

const VkCommandBuffer& CommandPool::operator[](const uint32_t index) const
{
    return commandBuffers[index];
}

VkCommandBuffer& CommandPool::operator[](const uint32_t index)
{
    return commandBuffers[index];
}

void CommandPool::destroy()
{
    if(pool)
    {
        vkDestroyCommandPool(system->getDevice(), pool, nullptr);
        pool = 0;
    }
    commandBuffers.clean();
}

CommandPool::~CommandPool()
{
    destroy();
}