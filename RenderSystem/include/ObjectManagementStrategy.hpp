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
    virtual void pickDepthStencilFormat(VkFormat& format, VkImageTiling& tiling) const = 0;
    virtual void pickImageFormat(VkFormat& format, VkImageTiling& tiling) const = 0;
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
    void pickDepthStencilFormat(VkFormat& format, VkImageTiling& tiling) const;
    void pickImageFormat(VkFormat& format, VkImageTiling& tiling) const;
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
        DLSampledImageFrag,
        DLUniformFrag,
        DLUniformVertTeseGeom,
        DLCount
    };
    enum MemoryObjects{MOTransfer, MOImage, MOBuffer, MOCount};
    enum Buffers{BVertex, BIndex, BTransfer, BUniform, BCount};

    struct BufferDescriptorUpdateCommand
    {
        const BufferInfo* buffer;
        uint32_t set;
        uint32_t binding;
        uint32_t arrayElement;
    };

    struct ImageDescriptorUpdateCommand
    {
        const SampledImageInfo* image;
        VkImageLayout layout;
        uint32_t set;
        uint32_t binding;
        uint32_t arrayElement;
        uint32_t descriptorCount;
    };

    struct InitialImageLayoutUpdateCommand
    {
        ImageInfo* image;
        VkImageLayout newLayout;
        VkImageSubresourceRange subresource;
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

    struct ViewCreateCommand
    {
        uint32_t index;
        uint32_t imageIndex;
        VkImageViewType type;
        VkFormat format;
        VkImageSubresourceRange subresource;
    };

    void createDescriptorLayouts();
    void preloadDescriptorSets();
    void allocateTransferBuffer();

    const System* system;
    VkPhysicalDeviceProperties deviceProperties;
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
    std::vector<VkMemoryRequirements> memoryRequirements[MemoryObjects::MOCount];
    uint32_t vertexBufferSize = 0;
    uint32_t indexBufferSize = 0;
    uint32_t uniformBufferSize = 0;
    uint32_t currentDescriptorCount = 0;
    std::vector<ViewCreateCommand> viewCreateCommands;
    std::vector<BufferDescriptorUpdateCommand> bufferDescriptorUpdateCommands;
    std::vector<ImageDescriptorUpdateCommand> imageDescriptorUpdateCommands;
    std::vector<BufferUpdateCommand> bufferUpdateCommands;
    std::vector<ImageUpdateCommand> imageUpdateCommands;
    std::vector<InitialImageLayoutUpdateCommand> layoutUpdateCommands;
    void* mappedTransferMemory = nullptr;
};

#endif