#include<ObjectManager.hpp>

ObjectManager::ObjectManager(){}

void ObjectManager::create(const System* system)
{
    this->system = system;
    createBuffers();
    createImages();
    allocateMemory();
}

void ObjectManager::destroy()
{
    //...
}

ObjectManager::~ObjectManager()
{
    destroy();
}

void ObjectManager::createBuffers()
{
    bufferPool.create(system, Buffers::Count);
    VkDeviceSize transferBufferSize, vertexIndexBufferSize, uniformBufferSize;
    bufferPool.createBuffer(Buffers::Transfer, transferBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    bufferPool.createBuffer(Buffers::VertexIndex, vertexIndexBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    bufferPool.createBuffer(Buffers::Uniform, uniformBufferSize, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

void ObjectManager::createImages()
{
}

void ObjectManager::allocateMemory();