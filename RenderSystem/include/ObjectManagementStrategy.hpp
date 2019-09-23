#ifndef OBJECT_MANAGEMENT_STRATEGY
#define OBJECT_MANAGEMENT_STRATEGY
#include<BufferHolder.hpp>
#include<System.hpp>
#include<CommandPool.hpp>
#include<Constants.hpp>
#include<DescriptorLayoutHolder.hpp>
#include<DescriptorPool.hpp>
#include<ImageLoader.hpp>
#include<ImageHolder.hpp>
#include<MemoryPool.hpp>
#include<SynchronizationPool.hpp>
#include<Utils.hpp>

class ObjectManagementStrategy
{
public:
    virtual void create(const System* system, 
        SynchronizationPool* syncPool, 
        CommandPool* commandPool) = 0;  // must be dynamic
    virtual void allocateSampledImage(const VkExtent3D& extent, SampledImageInfo& sampledImage, DescriptorInfo& sampledImageDescriptor) = 0;
    virtual void allocateDepthMap(const VkExtent2D& extent, ImageInfo& depthMap) = 0;
    virtual void allocateVertexBuffer(const uint32_t size, BufferInfo& buffer) = 0;
    virtual void allocateIndexBuffer(const uint32_t size, BufferInfo& buffer) = 0;
    virtual void allocateUniformBuffer(const uint32_t size, const VkShaderStageFlags stages, BufferInfo& buffer, DescriptorInfo& uniformDescriptor) = 0;
    virtual void updateBuffer(const void* src, const BufferInfo& dst) = 0;
    virtual void updateImage(const ImageLoader::Image& src, const ImageInfo& dst) = 0;
    virtual const VkPipelineLayout& getPipelineLayout(const DrawableType type) = 0;
    virtual void load() = 0;
    virtual void update() = 0;
    virtual void destroy() = 0;
};

class SharedMemoryObjectManagementStrategy : public ObjectManagementStrategy
{
public:
    SharedMemoryObjectManagementStrategy();
    void create(const System* system, 
        SynchronizationPool* syncPool, 
        CommandPool* commandPool);  // must be dynamic
    void allocateSampledImage(const VkExtent3D& extent, SampledImageInfo& sampledImage, DescriptorInfo& sampledImageDescriptor);
    void allocateDepthMap(const VkExtent2D& extent, ImageInfo& depthMap);
    void allocateVertexBuffer(const uint32_t size, BufferInfo& buffer);
    void allocateIndexBuffer(const uint32_t size, BufferInfo& buffer);
    void allocateUniformBuffer(const uint32_t size, const VkShaderStageFlags stages, BufferInfo& buffer, DescriptorInfo& uniformDescriptor);
    void updateBuffer(const void* src, const BufferInfo& dst);
    void updateImage(const ImageLoader::Image& src, const ImageInfo& dst);
    const VkPipelineLayout& getPipelineLayout(const DrawableType type);
    void load();
    void update();
    void destroy();
    ~SharedMemoryObjectManagementStrategy();
private:
    enum DescriptorLayouts
    {
        SampledImageFrag,
        UniformFrag,
        UniformVertTeseGeom,
        Count
    };
    enum MemoryObjects{Transfer, Image, Buffer, Count};
    enum Buffers{Vertex, Index, Transfer, Uniform, Count};

    struct BufferDescriptorUpdateCommand
    {
        const BufferInfo* buffer;
        uint32_t set;
        uint32_t binding;
        uint32_t arrayElement;
    };

    struct BufferUpdateCommand
    {
        const void* src;
        const BufferInfo* dst;
    };

    struct ImageUpdateCommand
    {
        const ImageLoader::Image* src;
        const ImageInfo* dst;
    };

    void createDescriptorLayouts();
    void preloadDescriptorSets();
    void pickDepthStencilFormat(VkFormat& format, VkImageTiling& tiling) const;
    void pickImageFormat(VkFormat& format, VkImageTiling& tiling) const;
    void allocateTransferBuffer();

    const System* system;
    MemoryPool memoryPool;
    BufferHolder bufferHolder;
    ImageHolder imageHolder;
    SynchronizationPool* syncPool;
    CommandPool* commandPool;
    DescriptorLayoutHolder descriptorLayoutHolder;
    DescriptorPool descriptorPool;
    uint32_t updateSemaphore;
    uint32_t updateFence;
    uint32_t updateCommandBuffer;
    bool firstCommandBufferRun = true;
    std::vector<VkMemoryRequirements> memoryRequirements[MemoryObjects::Count];
    uint32_t vertexBufferSize;
    uint32_t indexBufferSize;
    uint32_t uniformBufferSize;
    uint32_t currentDescriptorCount;
    std::vector<BufferDescriptorUpdateCommand> bufferDescriptorUpdateCommands;
    std::vector<BufferUpdateCommand> bufferUpdateCommands;
    std::vector<ImageUpdateCommand> imageUpdateCommands;
    void* mappedTransferMemory;
};

#endif