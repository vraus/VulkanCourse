#pragma once

// std
#include <stdexcept>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// src
#include "Utilites.h"

class VulkanRenderer
{
public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

    int init(GLFWwindow* new_window);
    void cleanup() const;

// Vulkan Functions
private:
    // Create Once functions
    void createInstance();
    void createLogicalDevice();
    void createSurface();
    void createSwapchain();
    void createGraphicsPipeline();

    // Creat Utilities functions
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;
    VkShaderModule createShaderModule(const std::vector<char>& code) const;
    
    // Getters
    void getPhysicalDevice();
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice physicalDevice) const;
    SwapchainSupportDetails getSwapchainDetails(VkPhysicalDevice physicalDevice) const;
    
    // Helpers
    bool checkPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
    bool checkInstanceExtensionSupport(const std::vector<const char*>* checkExtensions);
    bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

    // Choosers
    VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR chooseBestPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

private:
    // Vulkan Components
    VkInstance instance;
    
    struct
    {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSurfaceKHR surface;
    
    VkSwapchainKHR swapchain;
    std::vector<SwapchainImage> swapchainImages;

    // Vulkan Utilities
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    // glfw Components
    GLFWwindow* window;
};
