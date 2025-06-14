#include "../Public/VulkanRenderer.h"

int VulkanRenderer::init(GLFWwindow* new_window)
{
    window = new_window;

    try
    {
        createInstance();
        createSurface();
        getPhysicalDevice();
        createLogicalDevice();
        createSwapchain();
    } catch (const std::runtime_error &e)
    {
        std::cout << "ERROR:" << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return 0;
}

void VulkanRenderer::cleanup() const
{
    vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(mainDevice.logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
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
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create a Vulkan Instance!");
}

void VulkanRenderer::createLogicalDevice()
{
    constexpr float priority = 1.0f;
    
    // Get the queue family indices from the main device physical device
    const QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    // Vector for queue creation information, and set for unique family indices
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos; 
    const std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    // Queues the logical device needs to create and info to do so (only 1 for now...)
    for (const int queuFamilyIndex : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queuFamilyIndex;         // The index of the family to create a queue from
        queueCreateInfo.queueCount = 1;                             // The number of queue to create
        queueCreateInfo.pQueuePriorities = &priority;               // Vulkan needs to know how to prioritize all the queues

        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Physical device features the Logical Device will be using
    VkPhysicalDeviceFeatures deviceFeatures = {};
    
    // Creation information for the logical device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());        // Number of enabled logical device extensions
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();                             // List of enabled logical device extensions
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;                                            // Physical Device Features Logical Device will use

    // Create the logical device for the given physical device
    if (vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    // Queues are created at the same time as the device...
    // So we want handle to queues
    // Given logical device of given queue family of given queue index (here 0) place reference in given graphics queue handle
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentFamily, 0, &presentQueue);
}

void VulkanRenderer::createSurface()
{
    // Create surface
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}

void VulkanRenderer::createSwapchain()
{
    // Get swapchain details so we can pick the best settings
    SwapchainSupportDetails swapchainSupport = getSwapchainDetails(mainDevice.physicalDevice);

    // Find optimal surface values for our swapchain
    VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode = chooseBestPresentMode(swapchainSupport.presentationModes);
    VkExtent2D extent = chooseExtent(swapchainSupport.surfaceCapabilities);

    // Make sur we have enough images in the swapchain to allow triple buffering
    // While also making sur we are not going over the maximum
    uint32_t swapchainImageCount = swapchainSupport.surfaceCapabilities.minImageCount + 1;
    
    // if == 0 then it means there is no maximum value
    if (swapchainSupport.surfaceCapabilities.maxImageCount > 0 
        && swapchainSupport.surfaceCapabilities.maxImageCount < swapchainImageCount)
        swapchainImageCount = swapchainSupport.surfaceCapabilities.maxImageCount;

    // Swapchain create info struct
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;                                                      // Surface
    swapchainCreateInfo.minImageCount = swapchainImageCount;                                    // Images in the swapchain
    swapchainCreateInfo.imageFormat = surfaceFormat.format;                                     // Format
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;                             // Color Space
    swapchainCreateInfo.imageExtent = extent;                                                   // Extent
    swapchainCreateInfo.imageArrayLayers = 1;                                                   // Number of layers for each image in chain
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;                       // What attachment images will be used as
    swapchainCreateInfo.preTransform = swapchainSupport.surfaceCapabilities.currentTransform;   // Transform to perform on swapchain
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;                     // How to handle blending images with external graphics (e.g. windows ...)
    swapchainCreateInfo.presentMode = presentMode;                                              // Presentation Mode
    swapchainCreateInfo.clipped = VK_TRUE;                                                      // Whether to clip parts of image not in view (e.g. overlapped or offscreen)

    // Get Queue Family Indices
    QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

    // If Graphics and Presentation families are different, the swapchain must let images be shared between families
    if (indices.graphicsFamily != indices.presentFamily)
    {
        uint32_t queueFamilyIndices[] = {
            static_cast<uint32_t>(indices.graphicsFamily),
            static_cast<uint32_t>(indices.presentFamily),
        };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;              // Image share handling
        swapchainCreateInfo.queueFamilyIndexCount = 2;                                  // Number of queues to share images between
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;                   // Array of queues to share between
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;               // Images are exclusive to a single queue
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    }

    // This is null for now, but when we resize the window we destroy the old swapchain and pass the old to the new
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
        throw std::runtime_error("failed to create a Swapchain!");
}

void VulkanRenderer::getPhysicalDevice()
{
    // Enumerate Physical Devices the vkInstance can access
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, nullptr);

    // Check if at least one physical device is detected and supports Vulkan
    if (physicalDevicesCount == 0)
        throw std::runtime_error("failed to get physical devices!");

    // Store all the Physical Devices in the list
    std::vector<VkPhysicalDevice> physicalDevicesList(physicalDevicesCount);
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevicesList.data());

    for (const auto &device : physicalDevicesList)
    {
        if (checkPhysicalDeviceSuitable(device))
        {
            mainDevice.physicalDevice = device;
            break;
        }
    } 
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(const VkPhysicalDevice physicalDevice) const
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

        // Check if queue family supports presentation
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);
        
        // Check if queue is also presentation type (can be both presentation and graphics)
        if (queueFamily.queueCount > 0 && presentationSupport)
            indices.presentFamily = i;

        // If we ever reach a point where the queue family is valid then we can stop checking and break through
        if (indices.isValid())
            break;
        
        ++i;
    }

    return indices;
}

SwapchainSupportDetails VulkanRenderer::getSwapchainDetails(const VkPhysicalDevice physicalDevice) const
{
    SwapchainSupportDetails details;

    // -- CAPABILITIES --
    // Get the surface capabilities for the given surface on the given physical device
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.surfaceCapabilities);
    
    // -- FORMATS --
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());   
    }

    // -- PRESENTATION MODES -- 
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentationModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentationModes.data());
    }
    
    return details;
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

    QueueFamilyIndices indices = getQueueFamilies(physicalDevice);
    bool extensionsSupported = checkDeviceExtensionSupport(physicalDevice);

    bool swapChainValid = false;
    
    if (extensionsSupported)
    {
        SwapchainSupportDetails swapChainDetails = getSwapchainDetails(physicalDevice);
        swapChainValid = !swapChainDetails.presentationModes.empty() && !swapChainDetails.formats.empty();
    }
    
    return indices.isValid() && extensionsSupported && swapChainValid;
}

bool VulkanRenderer::checkInstanceExtensionSupport(const std::vector<const char*>* checkExtensions)
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

bool VulkanRenderer::checkDeviceExtensionSupport(const VkPhysicalDevice physicalDevice)
{
    // Get device extension count
    uint32_t glfwExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &glfwExtensionCount, nullptr);

    // If no extension found return false
    if (glfwExtensionCount == 0)
        return false;

    // Populate list of extensions
    std::vector<VkExtensionProperties> availableExtensions(glfwExtensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &glfwExtensionCount, availableExtensions.data());

    // Check for extensions
    for (const auto &deviceExtension : deviceExtensions)
    {
        bool hasExtension = false;

        for (const auto &extension : availableExtensions)
        {
            if (strcmp(extension.extensionName, deviceExtension) != 0)
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

/// Best format is subjective but ours will be:
/// - Format: VK_FORMAT_R8G8B8A8_UNORM (|| VK_FORMAT_B8G8R8A8_UNORM)
/// - ColorSpace (range of colors): VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
    // If this is true then ALL the formats are available
    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        return { VK_FORMAT_R8G8B8A8_UNORM,  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

    // If restricted find the optimal format
    for (const auto &format : formats)
    {
        if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }

    return formats[0];
}

VkPresentModeKHR VulkanRenderer::chooseBestPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
{
    // Look for mailbox presentation mode
    for (const auto &presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return presentMode;
    }

    return VK_PRESENT_MODE_FIFO_KHR; // This is a Vulkan specification, this present mode ALWAYS has to be available. Use it as a safe backup
}

VkExtent2D VulkanRenderer::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    // If current extent is at numeric limits, then extent can vary. Otherwise it is the size of the window.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;
    else
    {
        // If value can vary, need to set manually

        // Get window size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        // Create new extent using window size
        VkExtent2D extent = {};
        extent.width = static_cast<uint32_t>(width);
        extent.height = static_cast<uint32_t>(height);

        // Surface also defines max and min so make sure within boundaries by clamping value
        extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
        extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));

        return extent;
    }
}
