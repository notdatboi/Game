#ifndef COMMAND_POOL_HPP
#define COMMAND_POOL_HPP
#include<System.hpp>
#include<vector>

class CommandPool
{
public:
    CommandPool();
    const VkCommandPool& getPool() const;
    void create(const System* system, const bool dynamicPool = false);
    void addCommandBuffers(uint32_t count);
    const size_t getCurrentPoolSize() const;
    void allocateCommandBuffers(const uint32_t first, const uint32_t count, const VkCommandBufferLevel level);
    void reset(const uint32_t index, const bool releaseResources = false) const;
    const VkCommandBuffer& operator[](const uint32_t index) const;
    VkCommandBuffer& operator[](const uint32_t index);
    void destroy();
    ~CommandPool();
private:
    const System* system;
    VkCommandPool pool;
    std::vector<VkCommandBuffer> commandBuffers;
};

#endif