#ifndef OBJECT_MANAGER_HPP
#define OBJECT_MANAGER_HPP
#include<Constants.hpp>
#include<BufferPool.hpp>
#include<MemoryPool.hpp>
#include<ImageLoader.hpp>
#include<ImagePool.hpp>

class ObjectManager
{
public:
    ObjectManager();
    void create(const System* system);
    void destroy();
    ~ObjectManager();
private:
    void createBuffers();
    void createImages();
    void allocateMemory();
    const System* system;
    BufferPool bufferPool;
    ImagePool imagePool;
    MemoryPool MemoryPool;
};


#endif