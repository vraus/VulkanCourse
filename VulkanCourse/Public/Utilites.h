#pragma once

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

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities;       // Surface properties (Image size / extent, etc...)
    std::vector<VkSurfaceFormatKHR> formats;            // List of the supported image formats (Format, image data representation, color space)
    std::vector<VkPresentModeKHR> presentationModes;    // How images should be presented to screen
};