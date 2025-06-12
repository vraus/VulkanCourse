#include "../Public/VulkanRenderer.h"

int VulkanRenderer::init(GLFWwindow* new_window)
{
    window_ = new_window;

    try
    {
        create_instance();
    } catch (const std::runtime_error &e)
    {
        std::cout << "ERROR:" << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return 0;
}

void VulkanRenderer::create_instance()
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
    std::vector<const char*> instanceExtensions = std::vector<const char*>();

    uint32_t glfwExtensionCount = 0;                                                        // GLFW may require multiple extensions
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);   // Extensions passed as array of cstrings

    for (size_t i = 0; i < glfwExtensionCount; ++i)
        instanceExtensions.push_back(glfwExtensions[i]);
    
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
