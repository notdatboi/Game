#include<Utils.hpp>
#include<stdexcept>
#include<iostream>

void reportError(const char* error)
{
    throw std::runtime_error(error);
}

void printLog(const char* log)
{
    std::cout << log;
}

void checkResult(const VkResult& result, const char* error)
{
    if(result != VK_SUCCESS) reportError(error);
}