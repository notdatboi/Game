#ifndef UTILS_HPP
#define UTILS_HPP
#include<initializer_list>
#include<vulkan/vulkan.h>

struct ShaderStageInfo
{
    VkShaderStageFlagBits stage;
    VkShaderModule module;
};

template<typename T>
class Array
{
public:
    Array(): array(nullptr), size(0) {}
    
    Array(const std::initializer_list<T>& lst)
    {
        create(lst.size());
        auto listIterator = std::begin(lst);
        for(unsigned int ind = 0; ind < size; ++ind)
        {
            (*this)[ind] = *listIterator;
            ++listIterator;
        }
    }
    
    Array(const unsigned int size)
    {
        create(size);
    }

    Array(const Array& other)
    {
        *this = other;
    }

    Array(Array&& other)
    {
        clean();
        array = other.array;
        size = other.size;
        other.array = nullptr;
        other.size = 0;
    }

    void create(const unsigned int size, const T& defaultElement)
    {
        create(size);
        for(auto ind = 0; ind < size; ++ind)
        {
            *(array + ind) = defaultElement;
        }
    }

    void create(const unsigned int size = 0)
    {
        clean();
        array = new T[size];
        this->size = size;
    }

    void create(const unsigned int size, const T* data)
    {
        create(size);
        for(int ind = 0; ind < size; ++ind)
        {
            (*this)[ind] = *(data + ind);
        }
    }

    const T& operator[](const unsigned int index) const
    {
        return *(array + index);
    }

    T& operator[](const unsigned int index)
    {
        return *(array + index);
    }

    Array& operator=(const Array& other)
    {
        clean();
        create(other.size);
        for(unsigned int ind = 0; ind < size; ++ind)
        {
            *(array + ind) = other[ind];
        }
        return *this;
    }

    Array& operator=(Array&& other)
    {
        clean();
        array = other.array;
        size = other.size;
        other.array = nullptr;
        other.size = 0;
        return *this;
    }

    Array& operator=(const std::initializer_list<T>& lst)
    {
        clean();
        create(lst.size());
        auto listIterator = std::begin(lst);
        for(unsigned int ind = 0; ind < size; ++ind)
        {
            (*this)[ind] = *listIterator;
            ++listIterator;
        }
    }

    const bool empty() const
    {
        return size == 0;
    }

    const unsigned int getSize() const
    {
        return size;
    }

    const T* getPtr() const
    {
        return array;
    }

    T* getPtr()
    {
        return array;
    }

    void clean()
    {
        if(array)
        {
            delete[] array;
            size = 0;
        }
    }

    ~Array()
    {
        clean();
    }
private:
    T* array;
    unsigned int size;
};

void reportError(const char* error);

void checkResult(const VkResult& result, const char* error);

#endif