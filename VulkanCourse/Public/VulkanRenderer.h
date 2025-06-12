#pragma once

// std
#include <stdexcept>
#include <vector>
#include <iostream>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VulkanRenderer
{
public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

    int init(GLFWwindow* new_window);

private:
    // Vulkan Functions
    void create_instance();

    // Vulkan Components
    VkInstance instance_;

    GLFWwindow* window_;
};
