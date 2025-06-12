// std
#include <iostream>
#include <stdexcept>
#include <vector>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// src
#include "Public/VulkanRenderer.h"

// TODO: Anonymous Namespace will be then refactor into a VulkanWindow class and a VulkanApp class for clarity later
namespace
{
    GLFWwindow* window;
    VulkanRenderer vkRenderer;

    static void initWindow(const std::string& w_name = "Vulkan Course", const int width = 800, const int height = 600)
    {
        glfwInit();

        // Set glfw to not work with another graphic API other than Vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, w_name.c_str(), nullptr, nullptr);
    }

    static void shutdownApplication()
    {
        vkRenderer.cleanup();

        // Destroy GLFW Window and stop GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

int main()
{
    // Create our window
    initWindow();

    // Create Renderer instance
    if (vkRenderer.init(window) == EXIT_FAILURE)
        return EXIT_FAILURE;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    shutdownApplication();

    return 0;
}
