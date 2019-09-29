#include<System.hpp>
#include<vector>
#include<cstring>

System::System()
{
}

void System::create(const Window& window, const bool enableDebug, const VkPhysicalDeviceFeatures& enabledFeatures)
{
    uint32_t count;
    const char** ext;
    ext = window.getVulkanExtensions(count);
    createInstance(ext, count, enableDebug);
    surface = window.getVulkanSurface(instance);
    createDevice(enabledFeatures);
}

const VkSurfaceCapabilitiesKHR System::getSurfaceCapabilities() const
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);
    return capabilities;
}

const VkPhysicalDevice& System::getPhysicalDevice() const
{
    return physicalDevice;
}

const VkDevice& System::getDevice() const
{
    return device;
}

const QueueInfo& System::getPresentQueue() const
{
    return presentQueue;
}

const QueueInfo& System::getGraphicsQueue() const
{
    return graphicsQueue;
}

const VkSurfaceKHR& System::getSurface() const
{
    return surface;
}

void System::createInstance(const char** customExtensions, const uint32_t& extensionCount, const bool enableDebug)
{
    VkApplicationInfo appInfo = 
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Application",
        VK_MAKE_VERSION(1, 0, 0),
        "None",
        VK_MAKE_VERSION(0, 0, 0),
        VK_API_VERSION_1_0
    };

    std::vector<const char*> layers;
    const char* preferredLayer =
    //"VK_LAYER_LUNARG_api_dump";
    //"VK_LAYER_LUNARG_vktrace";
    //"VK_LAYER_LUNARG_device_simulation";
    //"VK_LAYER_LUNARG_monitor";
    //"VK_LAYER_LUNARG_screenshot";
    //"VK_LAYER_LUNARG_core_validation";
    //"VK_LAYER_GOOGLE_threading";
    "VK_LAYER_KHRONOS_validation";
    //"VK_LAYER_LUNARG_parameter_validation";
    //"VK_LAYER_LUNARG_standard_validation";
    //"VK_LAYER_GOOGLE_unique_objects";
    //"VK_LAYER_LUNARG_object_tracker";
    std::vector<const char*> extensions(customExtensions, customExtensions + extensionCount);
    if(enableDebug)
    {
        uint32_t supportedLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
        Array<VkLayerProperties> props(supportedLayerCount);
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, props.getPtr());
        for(uint32_t i = 0; i < supportedLayerCount; ++i)
        {
            if(strcmp(props[i].layerName, preferredLayer) == 0)
            {
                layers.resize(1);
                layers[0] = preferredLayer;
                break;
            }
        }
        
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo instanceInfo = 
    {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &appInfo,
        static_cast<uint32_t>(layers.size()),
        layers.data(),
        static_cast<uint32_t>(extensions.size()),
        extensions.data()
    };

    checkResult(vkCreateInstance(&instanceInfo, nullptr, &instance), "Instance was not created.\n");

    if(enableDebug) createDebugMessenger();
}

void System::pickPhysicalDevice()
{
    uint32_t count;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    Array<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.getPtr());
    for(uint32_t ind = 0; ind < count; ++ind)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(devices[ind], &properties);
        if(properties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            physicalDevice = devices[ind];
            break;
        }
    }
}

void System::pickQueueFamilies()
{
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    Array<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, properties.getPtr());
    graphicsQueue.familyIndex = ~0;
    presentQueue.familyIndex = ~0;
    for(uint32_t ind = 0; ind < count; ++ind)
    {
        if(properties[ind].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            VkBool32 surfaceSupported;
            checkResult(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, ind, surface, &surfaceSupported), "Failed to get device properties.\n");
            if(surfaceSupported)
            {
                graphicsQueue.familyIndex = ind;
                presentQueue.familyIndex = ind;
                break;
            }
            graphicsQueue.familyIndex = ind;
        }
        else
        {
            VkBool32 surfaceSupported;
            checkResult(vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, ind, surface, &surfaceSupported), "Failed to get device properties.\n");
            if(surfaceSupported)
            {
                presentQueue.familyIndex = ind;
            }
        }
    }
}

void System::createDevice(const VkPhysicalDeviceFeatures& enabledFeatures)
{
    pickPhysicalDevice();
    pickQueueFamilies();

    const float queuePriorities[1] = {1};
    uint32_t queueCount = 1;
    VkDeviceQueueCreateInfo queueInfos[2];
    // Graphics queue
    queueInfos[0] = 
    {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        graphicsQueue.familyIndex,
        1,
        queuePriorities
    };
    // Present queue
    if(graphicsQueue.familyIndex != presentQueue.familyIndex)
    {
        queueInfos[1] = 
        {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            nullptr,
            0,
            presentQueue.familyIndex,
            1,
            queuePriorities
        };
        queueCount = 2;
    }

    std::vector<const char*> extensions(1);
    extensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    
    VkDeviceCreateInfo deviceInfo = 
    {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        queueCount,
        queueInfos,
        0,
        nullptr,
        static_cast<uint32_t>(extensions.size()),
        extensions.data(),
        &enabledFeatures
    };
    checkResult(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device), "Device was not created.\n");
    obtainQueues();
}

void System::obtainQueues()
{
    vkGetDeviceQueue(device, graphicsQueue.familyIndex, 0, &graphicsQueue.queue);
    if(graphicsQueue.familyIndex != presentQueue.familyIndex)
    {
        vkGetDeviceQueue(device, presentQueue.familyIndex, 0, &presentQueue.queue);
    }
    else
    {
        presentQueue.queue = graphicsQueue.queue;
    }
}

void System::destroy()
{
    if(device)
    {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
        device = 0;
    }
    if(debugMessenger)
    {
        auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if(destroyFunc != nullptr) destroyFunc(instance, debugMessenger, nullptr);
        debugMessenger = 0;
    }
    if(instance)
    {
        vkDestroyInstance(instance, nullptr);
        instance = 0;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL System::callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData)
{
    if(severity > VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) reportError(data->pMessage);
    printLog("DEBUG CALLBACK: ");
    printLog(data->pMessage);
    printLog("\n");
    return VK_FALSE;
}

void System::createDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT info = 
    {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        nullptr,
        0,
        VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        callback,
        nullptr
    };
    auto creationFunc = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if(creationFunc == nullptr) reportError("Can't obtain debug messenger creation function.\n");
    checkResult(creationFunc(instance, &info, nullptr, &debugMessenger), "Failed to create debug messenger");
}

System::~System()
{
    destroy();
}