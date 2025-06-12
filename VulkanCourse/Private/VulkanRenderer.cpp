#include "../Public/VulkanRenderer.h"

int VulkanRenderer::init(GLFWwindow* new_window)
{
    window_ = new_window;

    try
    {
        createInstance();
        getPhysicalDevice();
        createLogicalDevice();
    } catch (const std::runtime_error &e)
    {
        std::cout << "ERROR:" << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return 0;
}

void VulkanRenderer::cleanup() const
{
    vkDestroyInstance(instance_, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
}

void VulkanRenderer::createInstance()
{
    // Create Info for Application
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Course";             // Custom Application name
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);  // Custom Application version
    appInfo.pEngineName = "No Engine";                      // Custom Engine name
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);       // Custom Engine version
    appInfo.apiVersion = VK_API_VERSION_1_4;                // Vulkan Version

    // Create list to hold instance extensions
    auto instanceExtensions = std::vector<const char*>();

    uint32_t glfwExtensionCount = 0;                                                        // GLFW may require multiple extensions
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);   // Extensions passed as array of cstrings

    for (size_t i = 0; i < glfwExtensionCount; ++i)
        instanceExtensions.push_back(glfwExtensions[i]);

    if (!checkInstanceExtensionSupport(&instanceExtensions))
        throw std::runtime_error("Required extensions not supported by VkInstance!");
    
    // Create Info for VkInstance
    VkInstanceCreateInfo instanceCreateInfo = {};    
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledLayerCount = 0;   // TODO: Setup a validation Layers that Instance will use
    instanceCreateInfo.ppEnabledLayerNames = nullptr;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // Create Instance
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance_) != VK_SUCCESS)
        throw std::runtime_error("failed to create a Vulkan Instance!");
}

void VulkanRenderer::createLogicalDevice()
{
    constexpr float priority = 1.0f;
    
    // Get the queue family indices from the main device physical device
    const QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);
    
    // Queue the logical device needs to create and info to do so (only 1 for now...)
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;  // The index of the family to create a queue from
    queueCreateInfo.queueCount = 1;                             // The number of queue to create
    queueCreateInfo.pQueuePriorities = &priority;               // Vulkan needs to know how to prioritize all the queues

    // Physical device features the Logical Device will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    // Creation information for the logical device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = 0;                 // Number of enabled logical device extensions
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;         // List of enabled logical device extensions
    deviceCreateInfo.pEnabledFeatures = nullptr;                // No features for now in the device
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;        // Physical Device Features Logical Device will use

    // Create the logical device for the given physical device
    if (vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    // Queues are created at the same time as the device...
    // So we want handle to queues
    // Given logical device of give queue family of givec queue index (here 0) place reference in given graphics queue handle
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

void VulkanRenderer::getPhysicalDevice()
{
    // Enumerate Physical Devices the vkInstance can access
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(instance_, &physicalDevicesCount, nullptr);

    // Check if at least one physical device is detected and supports Vulkan
    if (physicalDevicesCount == 0)
        throw std::runtime_error("failed to get physical devices!");

    // Store all the Physical Devices in the list
    std::vector<VkPhysicalDevice> physicalDevicesList(physicalDevicesCount);
    vkEnumeratePhysicalDevices(instance_, &physicalDevicesCount, physicalDevicesList.data());

    for (const auto &device : physicalDevicesList)
    {
        if (checkPhysicalDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    } 
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(const VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices indices;

    // Get all Queue Family Property info for the given device
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    // Store the queue families in the list
    std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyList.data());

    // Go through each queue family and check if it has at least 1 of the required types of queue
    int i = 0;
    for (const auto &queueFamily : queueFamilyList)
    {
        // First check if queue family has at least 1 queue in that family (could have no queues)
        // Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to check if has required type
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;    // If queue family is valid then get the index

        // If we ever reach a point where the queue family is valid then we can stop checking and break through
        if (indices.isValid())
            break;
        
        ++i;
    }

    return indices;
}

bool VulkanRenderer::checkPhysicalDeviceSuitable(const VkPhysicalDevice physicalDevice)
{
    /* Commented for now. We will check properties and features later
    // Information about the device itself (ID, name, type, vendor, etc...)
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    // Information about what the device can do (geo shader, tess shader, wide lines, etc...)
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
     */

    // Check if queues are supported
    return getQueueFamilies(physicalDevice).isValid();
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
    // Need to get number of extensions to create array of correct size to hold extensions
    uint32_t glfwExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionCount, nullptr);

    // Create a list of VkExtensionProperties using count
    std::vector<VkExtensionProperties> availableExtensions(glfwExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &glfwExtensionCount, availableExtensions.data());

    // Check if given extensions are in list of available extensions
    for (const auto &chekExtension : *checkExtensions)
    {
        bool hasExtension = false;
        for (const auto &extension : availableExtensions)
        {
            if (strcmp(extension.extensionName, chekExtension) != 0)
            {
                hasExtension = true;
                break;
            }
        }

        if (!hasExtension)
            return false;
    }

    return true;
}
