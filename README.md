# VulkanCourse

> The Udemy Vulkan Course repo

## Setup

The project needs the following to be installed in order to work properly:
+ [OpenGlm](https://glm.g-truc.net)
+ [64-bit glfw](https://www.glfw.org/download.html)
+ [Vulkan SDK 1.4.313.2](https://vulkan.lunarg.com/sdk/home#windows)

## Instances, Devices and Validation

> Setting up Vulkan to use a device, and enabling Validation Layers to validate code

### Vulkan Instances

+ A reference to the Vulkan context
    + The first thing we create
    + It's the Vulkan program itself
+ Define the Vulkan version and its capabilities
    + Extensions
+ All Vulkan applications start by creating a Vulkan Instance
+ Physical Devices accessible to the Instance are enumerated and one or more are chosen ...
    + Basically choosing a GPU
+ Then the Instance creates a Logical Device to handle the rest of the work
+ Instances are rarely used beyond this

### Devices

+ **Two kinds of device:**
    + [Physical Device](#physical-device)
        + The **GPU** itself
        + Holds memory and queues to process pipelin
        + Can't be interacted with directly
    + [Logical Device](#logical-device)
        + An interface to the Physical Device
        + Will be used a lot
        + Through Logical Device we set up everything on the GPU
            + Creating swapchain etc... requieres this interface

#### Physical Device

+ A direct reference to the GPU itself
+ Contains two important aspects:
    + **Memory**: When we want to allocate memory to resources, it must be handled through the Physical Device.
    + [**Queues**](Queues): Process commands submitted to GPU in FIFO order. Different queues can be used for different types of command.
+ Physical Devices are "**retrived**" not created like most Vulkan concepts.
+ Do this by enumerating over all devices and picking suitable one.

#### Logical Device

+ Acts as an interface to the Physical Device.
+ Will be referenced in a lot of Vulkan functions
+ Most Vulkan objects are created on the device, and we use the reference to the logival Device to state which device to create those objects on.
+ Creation is relatively simple:
    + Define queue families and number of queues you wish to assign to the Logical Device from the Physical Device.
    + Define all the device features you wish to enable (e.g Geometry Shader, anisotropy, wide lines, etc...)
    + Define [**extensions**](#extensions) the device will use.
    + In the past you would define [**Validation Layers**](#validation-layers) too. As of Vulkan 1.1 this is deprecated.
        + They are now only defined in the instance, and also defined for the device

#### Queues

+ Physical Devices can have multiple types of queue.
+ Types are referred to as "Queue Families"
+ A family can have more than one queue in it
+ Example queue families:
    + **Graphics:** A family for processing graphics commands
    + **Compute:** A family for processing compute shaders (generic commands)
    + **Transfer:** A family for processing data transfer operations
+ Queue families can be and often are combinations of these!
+ When we enumerate Physical Devices, we need to check the device has the queue families we require for the application.

#### Extensions

+ By default, Vulkan has no understanding of what a "window" is.
+ Makes sense sice Vulkan is cross-platform and windows on different systems are defined differently.
+ Vulkan uses extensions to add window functionality.
+ These extensions are so commonly used that they come pre-packed with Vulkan anyway !
+ We can choose required extensions manually... But GLFW library has function to choose them for us !

#### GLFW

+ Graphics Library Framework
+ Originally designed for OpenGL but updated to work with Vulkan
+ Allows cross-platform window creation and automatic interfacing with OpenGL/Vulkan
+ Contains function that identifies the required Vulkan extensions for the host system, and returns a list of them !
+ `glfwGetRequiredInstanceExtensions(...);`
+ Can then use this list to set up Vulkan with the correct extensions

#### Validation Layers

+ By default, Vulkan does not validate code. Will not report errors, and will simply crash if it encounters a fatal error.
+ This is to avoid unnecessary overhead of error checking in release code.
    + Optimised
+ Must enable a Validation "Layer" to check manually.
+ Each "Layer" can check different functions.
    + For example `VK_LAYER_LUNARG_swapchain` validates Swapchain functionnality. `VK_LAYER_LUNARG_standard_validation` is a common all-round layer. 
+ Layers are similar to extensions and are not built-in to the core Vulkan code. Must be acquired from third parties.
+ Additionally, the reporting of validation errors is not a core Vulkan functions and will require another extension to be applied.
+ Note: Before Vulkan 1.1, validation layers could be specified separately for an Instance and a Logical Device. This is no longer the case, now the Instance validation layers cover both.