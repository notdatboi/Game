#ifndef COMMAND_POOL_HPP
#define COMMAND_POOL_HPP
#include<System.hpp>

class CommandPool
{
public:
    CommandPool();
    const VkCommandPool& getPool() const;
    void create(const System* system, const uint32_t commandBufferCount, const bool dynamicPool = false);
    void allocateCommandBuffers(const uint32_t first, const uint32_t count, const VkCommandBufferLevel& level);
    const VkCommandBuffer& operator[](const uint32_t index) const;
    VkCommandBuffer& operator[](const uint32_t index);
    void destroy();
    ~CommandPool();
private:
    const System* system;
    VkCommandPool pool;
    Array<VkCommandBuffer> commandBuffers;
};

#endif