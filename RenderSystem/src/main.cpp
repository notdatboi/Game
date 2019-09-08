#include<Swapchain.hpp>
#include<MemoryPool.hpp>

int main()
{
    glfwInit();
    Window window;
    window.create("TEST", {800, 600});
    System system;
    VkPhysicalDeviceFeatures features{};
    system.create(window, true, features);
    Array<VkMemoryRequirements> rq(1);
    rq[0] = 
    {
        2048,
        2048,
        (~0U)
    };
    MemoryPool pool;
    pool.create(&system, 1);
    pool.allocate(rq, 0);
    pool[0];
    pool.destroy();
    Swapchain swapchain;
    swapchain.create(&system);
    swapchain.destroy();
    system.destroy();
    glfwTerminate();
}