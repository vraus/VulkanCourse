#pragma once

// std
#include <stdexcept>
#include <vector>
#include <iostream>

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
    // Create functions
    void createInstance();

    // Getters
    void getPhysicalDevice();
    QueueFamilyIndices getQueueFamilies(VkPhysicalDevice physicalDevice);
    
    // Helpers
    bool checkPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice);
    bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions);

private:
    // Vulkan Components
    VkInstance instance_;
    struct
    {
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
    } mainDevice;

    // glfw Components
    GLFWwindow* window_;
};
