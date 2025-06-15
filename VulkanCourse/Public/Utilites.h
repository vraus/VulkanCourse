#pragma once

#include <fstream>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/// Indices (locations) of Queue Families (if they exist at all)
struct QueueFamilyIndices
{
    int graphicsFamily = -1; // Location of Graphics Queue Family
    int presentFamily = -1; // Location of Presentation Queue Family

    // Check if queue families are valid
    bool isValid() const
    {
        return graphicsFamily >= 0 && presentFamily >= 0;
    }
};

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;       // Surface properties (Image size / extent, etc...)
    std::vector<VkSurfaceFormatKHR> formats;            // List of the supported image formats (Format, image data representation, color space)
    std::vector<VkPresentModeKHR> presentationModes;    // How images should be presented to screen
};

struct SwapchainImage
{
    VkImage image;
    VkImageView imageView;
};

static std::vector<char> readFile(const std::string& filename)
{
    // Open stream from given file.
    // std::ios::binary : Read file as binary
    // std::ios::ate : Stream start readinf from the end of file
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    // Get current size of the file to create the file buffer
    size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    // Move read position to start of the file
    file.seekg(0);

    // Read the file data into the buffer
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

    // Close the stream
    file.close();
    
    return buffer;
}