// ReSharper disable CppClangTidyClangDiagnosticUnusedMacros

// std
#include <iostream>

// glfw
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// glm
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan Course", nullptr, nullptr);


    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << "Extension count :" << extensionCount << "\n";

    glm::mat4 testMatrix(1.0f);
    glm::vec4 testVector(1.0f);

    auto testResult = testMatrix * testVector;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
