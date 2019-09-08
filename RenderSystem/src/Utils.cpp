#include<Utils.hpp>
#include<stdexcept>

void reportError(const char* error)
{
    throw std::runtime_error(error);
}

void checkResult(const VkResult& result, const char* error)
{
    if(result != VK_SUCCESS) reportError(error);
}