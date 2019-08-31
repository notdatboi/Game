#include<System.hpp>
#include<string>

System::System()
{
}

void System::create(const Window& window, const bool enableDebug, const VkPhysicalDeviceFeatures& enabledFeatures)
{
    createInstance(window.getVulkanExtensions(), enableDebug);
    surface = window.getVulkanSurface(instance);
    createDevice(enabledFeatures);
}

void System::createSwapchain(Swapchain& swapchain) const
{
    uint32_t queueFamilyIndexCount = 0;
    if(graphicsQueue.familyIndex == presentQueue.familyIndex)
    {
        queueFamilyIndexCount = 1;
        const uint32_t queueFamilies[] = {graphicsQueue.familyIndex};
        swapchain.create(device, physicalDevice, surface, queueFamilyIndexCount, queueFamilies);
    }
    else
    {
        queueFamilyIndexCount = 2;
        const uint32_t queueFamilies[] = {graphicsQueue.familyIndex, presentQueue.familyIndex};
        swapchain.create(device, physicalDevice, surface, queueFamilyIndexCount, queueFamilies);
    }
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

void System::createInstance(const Array<const char*>& customExtensions, const bool enableDebug)
{
    VkApplicationInfo appInfo = 
    {
        VK_STRUCTURE_TYPE_APPLICATION_INFO,
        nullptr,
        "Application",
        VK_MAKE_VERSION(1, 0, 0),
        "None",
        VK_MAKE_VERSION(0, 0, 0),
        VK_MAKE_VERSION(1, 0, 0)
    };

    Array<const char*> layers = std::move(generateInstanceLayers(enableDebug));
    Array<const char*> extensions = std::move(generateInstanceExtensions(customExtensions, enableDebug));

    VkInstanceCreateInfo instanceInfo = 
    {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0,
        &appInfo,
        layers.getSize(),
        layers.getPtr(),
        extensions.getSize(),
        extensions.getPtr()
    };

    checkResult(vkCreateInstance(&instanceInfo, nullptr, &instance), "Instance was not created.\n");
}

Array<const char*> System::generateInstanceExtensions(const Array<const char*>& customExtensions, const bool enableDebug)
{
    unsigned int count = customExtensions.getSize() + enableDebug;
    Array<const char*> extensions;
    extensions.create(count);
    for(unsigned int i = 0; i < customExtensions.getSize(); ++i)
    {
        extensions[i] = customExtensions[i];
    }
    if(enableDebug) extensions[extensions.getSize() - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    return extensions;
}

Array<const char*> System::generateInstanceLayers(const bool enableDebug)
{
    Array<const char*> layers;
    if(enableDebug)
    {
        std::string preferredLayer = "VK_LAYER_KHRONOS_validation";
        uint32_t supportedLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);
        Array<VkLayerProperties> props(supportedLayerCount);
        vkEnumerateInstanceLayerProperties(&supportedLayerCount, props.getPtr());
        for(uint32_t i = 0; i < supportedLayerCount; ++i)
        {
            if(std::string(props[i].layerName) == preferredLayer)
            {
                layers.create(1);
                layers[0] = props[i].layerName;
                break;
            }
        }
    }
    return layers;
}

Array<const char*> System::generateDeviceExtensions()
{
    Array<const char*> ext(1);
    ext[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    return ext;
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
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, ind, surface, &surfaceSupported);
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
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, ind, surface, &surfaceSupported);
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
    queueInfos[1] = 
    {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        presentQueue.familyIndex,
        1,
        queuePriorities
    };

    auto extensions = std::move(generateDeviceExtensions());
    
    VkDeviceCreateInfo deviceInfo = 
    {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        2,
        queueInfos,
        0,
        nullptr,
        extensions.getSize(),
        extensions.getPtr(),
        &enabledFeatures
    };
    checkResult(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device), "Device was not created.\n");
}

System::~System()
{
    if(device)
    {
        vkDeviceWaitIdle(device);
        vkDestroyDevice(device, nullptr);
        device = 0;
    }
    if(instance)
    {
        vkDestroyInstance(instance, nullptr);
        instance = 0;
    }
}