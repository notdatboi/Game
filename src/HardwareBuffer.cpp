#include<HardwareBuffer.hpp>

namespace spk
{
    HardwareBuffer::HardwareBuffer()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const auto& commandPool = system::Executives::getInstance()->getPool();
        bufferMemoryData.index = (~0);
        bufferMemoryData.offset = (~0);
        shadowMemoryData.index = (~0);
        shadowMemoryData.offset = (~0);

        vk::CommandBufferAllocateInfo commandBufferInfo;
        commandBufferInfo.setCommandBufferCount(1)
            .setCommandPool(commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary);
        if(logicalDevice.allocateCommandBuffers(&commandBufferInfo, &commands) != vk::Result::eSuccess) throw std::runtime_error("Failed to allocate command buffer\n");
        vk::FenceCreateInfo fenceInfo;
        fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        if(logicalDevice.createFence(&fenceInfo, nullptr, &readyFence) != vk::Result::eSuccess) throw std::runtime_error("Failed to create fence!\n");
        vk::SemaphoreCreateInfo semaphoreInfo;
        if(logicalDevice.createSemaphore(&semaphoreInfo, nullptr, &readySemaphore) != vk::Result::eSuccess) throw std::runtime_error("Failed to create semaphore!\n");
    }

    void HardwareBuffer::setShadowBufferPolicy(bool use = false)
    {
        useShadowBuffer = use;
    }

    void HardwareBuffer::setAccessibility(const HardwareResourceAccessibility accessibility)
    {
        this->accessibility = accessibility;
    }

    void HardwareBuffer::setUsage(const vk::BufferUsageFlags usage)
    {
        this->usage = usage;
    }

    void HardwareBuffer::setSize(const uint32_t size)
    {
        this->size = size;
    }

    void HardwareBuffer::load()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        const uint32_t queueFamIndices[] = {system::Executives::getInstance()->getGraphicsQueueFamilyIndex()};
        vk::BufferCreateInfo bufferInfo;
        bufferInfo.setQueueFamilyIndexCount(1)
            .setPQueueFamilyIndices(queueFamIndices)
            .setSharingMode(vk::SharingMode::eExclusive)
            .setSize(size)
            .setUsage(usage);
        
        if(logicalDevice.createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) throw std::runtime_error("Failed to create buffer!\n");
        if(useShadowBuffer)
        {
            bufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
            if(logicalDevice.createBuffer(&bufferInfo, nullptr, &shadow) != vk::Result::eSuccess) throw std::runtime_error("Failed to create buffer!\n");
        }

        vk::MemoryRequirements bufferRequirements;
        logicalDevice.getBufferMemoryRequirements(buffer, &bufferRequirements);
        system::MemoryAllocationInfo memoryAllocationInfo(bufferRequirements);
        if(accessibility == HardwareResourceAccessibility::Dynamic)
        {
            memoryAllocationInfo.flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        }
        else
        {
            memoryAllocationInfo.flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
        }
        bufferMemoryData = system::MemoryManager::getInstance()->allocateMemory(memoryAllocationInfo);
        const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(bufferMemoryData.index);
        if(logicalDevice.bindBufferMemory(buffer, memory, bufferMemoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind buffer memory!\n");

        if(useShadowBuffer)
        {
            vk::MemoryRequirements shadowRequirements;
            logicalDevice.getBufferMemoryRequirements(shadow, &shadowRequirements);
            system::MemoryAllocationInfo shadowMemAllocInfo(shadowRequirements);
            shadowMemAllocInfo.flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            shadowMemoryData = system::MemoryManager::getInstance()->allocateMemory(shadowMemAllocInfo);
            const vk::DeviceMemory& shadowMemory = system::MemoryManager::getInstance()->getMemory(shadowMemoryData.index);
            if(logicalDevice.bindBufferMemory(shadow, shadowMemory, shadowMemoryData.offset) != vk::Result::eSuccess) throw std::runtime_error("Failed to bind buffer memory!\n");
        }

        loaded = true;
    }

    void HardwareBuffer::loadFromBuffer(const HardwareBuffer& buffer)
    {
        const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(shadowMemoryData.index);
        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();

        if(!loaded)
        {
            load();
        }

        vk::BufferCopy copyInfo;
        copyInfo.setSrcOffset(0)
            .setDstOffset(0)
            .setSize(size);
        
        waitUntilReady();
        resetWaiter();

        commands.reset(vk::CommandBufferResetFlags());
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        commands.begin(&beginInfo);
//        commands.copyBuffer(shadow, buffer, 1, &copyInfo);
        commands.copyBuffer(buffer.getVkBuffer(), this->buffer, 1, &copyInfo);
        if(useShadowBuffer)
        {
            commands.copyBuffer(buffer.getVkBuffer(), shadow, 1, &copyInfo);
        }
        commands.end();

        vk::PipelineStageFlags stageMask = vk::PipelineStageFlagBits::eTransfer;

        vk::SubmitInfo submit;

        if(waitForSemaphore)
        {
            submit.setWaitSemaphoreCount(1)
                .setPWaitSemaphores(&readySemaphore);
        }
        else
        {
            submit.setWaitSemaphoreCount(0)
                .setPWaitSemaphores(nullptr);
            waitForSemaphore = true;
        }

        submit.setPWaitDstStageMask(&stageMask)
            .setCommandBufferCount(1)
            .setPCommandBuffers(&commands)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(&readySemaphore);

        const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();
        graphicsQueue.submit(1, &submit, readyFence);
    }

    void HardwareBuffer::loadFromMemory(const void* data)
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        if(!loaded)
        {
            load();
        }

        if(accessibility == HardwareResourceAccessibility::Dynamic)
        {
            const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(bufferMemoryData.index);
            void* mapped;
            if(logicalDevice.mapMemory(memory, bufferMemoryData.offset, size, vk::MemoryMapFlags(), &mapped) != vk::Result::eSuccess) throw std::runtime_error("Failed to map memory!\n");
            memcpy(mapped, data, size);
            logicalDevice.unmapMemory(memory);
            // Let's just completely ignore the shadow buffer until I find out, why would we need it for CPU-accessible buffer
        }
        else
        {
            if(useShadowBuffer)
            {
                const vk::DeviceMemory& memory = system::MemoryManager::getInstance()->getMemory(shadowMemoryData.index);
                void* mapped;
                if(logicalDevice.mapMemory(memory, shadowMemoryData.offset, size, vk::MemoryMapFlags(), &mapped) != vk::Result::eSuccess) throw std::runtime_error("Failed to map memory!\n");
                memcpy(mapped, data, size);
                logicalDevice.unmapMemory(memory);

                vk::BufferCopy copyInfo;
                copyInfo.setSrcOffset(0)
                    .setDstOffset(0)
                    .setSize(size);

                waitUntilReady();
                resetWaiter();

                commands.reset(vk::CommandBufferResetFlags());
                vk::CommandBufferBeginInfo beginInfo;
                beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
                commands.begin(&beginInfo);
                commands.copyBuffer(shadow, buffer, 1, &copyInfo);
                commands.end();

                vk::PipelineStageFlags stageMask = vk::PipelineStageFlagBits::eTransfer;

                vk::SubmitInfo submit;

                if(waitForSemaphore)
                {
                    submit.setWaitSemaphoreCount(1)
                        .setPWaitSemaphores(&readySemaphore);
                }
                else
                {
                    submit.setWaitSemaphoreCount(0)
                        .setPWaitSemaphores(nullptr);
                    waitForSemaphore = true;
                }

                submit.setPWaitDstStageMask(&stageMask)
                    .setCommandBufferCount(1)
                    .setPCommandBuffers(&commands)
                    .setSignalSemaphoreCount(1)
                    .setPSignalSemaphores(&readySemaphore);

                const auto& graphicsQueue = system::Executives::getInstance()->getGraphicsQueue();
                graphicsQueue.submit(1, &submit, readyFence);
            }
            else throw std::runtime_error("Device memory can't be accessed without shadow buffer.\n");
        }
    }

    void HardwareBuffer::waitUntilReady() const
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        if(logicalDevice.waitForFences(1, &readyFence, true, ~0U) != vk::Result::eSuccess) throw std::runtime_error("Failed to wait for image to be ready.\n");
    }

    void HardwareBuffer::resetWaiter()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        logicalDevice.resetFences(1, &readyFence);
    }

    const vk::Buffer& HardwareBuffer::getVkBuffer() const
    {
        return buffer;
    }

    vk::Buffer& HardwareBuffer::getVkBuffer()
    {
        return buffer;
    }

    void HardwareBuffer::clearResources()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();
        waitUntilReady();
        commands.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
        if(bufferMemoryData.index != (~0) && bufferMemoryData.offset != (~0))
        {
            system::MemoryManager::getInstance()->freeMemory(bufferMemoryData.index);
            bufferMemoryData.index = (~0);
            bufferMemoryData.offset = (~0);
        }
        if(buffer)
        {
            logicalDevice.destroyBuffer(buffer, nullptr);
            buffer = vk::Buffer();
        }
        if(useShadowBuffer)
        {
            if(shadowMemoryData.index != (~0) && shadowMemoryData.offset != (~0))
            {
                system::MemoryManager::getInstance()->freeMemory(shadowMemoryData.index);
                shadowMemoryData.index = (~0);
                shadowMemoryData.offset = (~0);
            }
            if(shadow)
            {
                logicalDevice.destroyBuffer(shadow, nullptr);
                shadow = vk::Buffer();
            }
        }
        loaded = false;
    }

    HardwareBuffer::~HardwareBuffer()
    {
        const auto& logicalDevice = system::System::getInstance()->getLogicalDevice();

        clearResources();
        if(readyFence)
        {
            logicalDevice.destroyFence(readyFence, nullptr);
            readyFence = vk::Fence();
        }
        if(readySemaphore)
        {
            logicalDevice.destroySemaphore(readySemaphore, nullptr);
            readySemaphore = vk::Semaphore();
        }
    }
}