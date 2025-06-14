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

## Surfaces, Swapchains and ImageViews

### More Extensions !

+ As mentioned previously: Displaying images to a window is not native to Vulkan. Requires extensions.
+ This also includes swapping out images like in a Double/Triple Buffer.
    + This means the common method of drawing to one image while another is displayed, and the swapping them at an appropriate time to display the updated image.
+ To do this we need two things:
    + **[Swapchain](#the-swapchain)**: Complex object that handles retrieval and updating of images being displayed, or yet to be.
    +  **[Surface](#surface)**: An interface between the window and an image in the swapchain.

#### The Swapchain 

+ Another extension. Will need to be checked for compatibility.
+ The swapchain is a group of images that can be drawn to and presented
+ The object is set up so it can be queried to get a new image to be drawn to and then presented. 
+ This requires a lot of synchronisation... Which we will cover in a later video!
+ The Swapchain has a few parts that need to be setup

##### Creating the Swapchain

+ A Swapchain has 3 major parts:
    + **Surface Capabilites**: What the surface is capable of handling
    + **Surface Format**: The format of the surface the Swapchain will presnet to (e.g. RGB)
    + **[Presentation Mode](#presentation-mode)**: The order and timing of images being presented to the surface
+ The settings are based on the Surface. We query the Surface for these parts to ensure compatibility.

##### Presentation Mode

+ Presentation Modes are the order and timing of images being presented to the Surface.
+ There are 4 modes and 2 are often considered undesirable due to the risk of tearing.
+ At any one time there will be **one image** present at the Surface that is being drawn to the screen. ***(Not 0, Not 2, 1)***.
+ First, Some information about how monitors work:
    + Monitors draw starting at the top-left pixel, and then drawing to the screen row-by-row.
    + When the image completes, the screen is then cleared to start drawing again, the period of time AFTER this clear and BEFORE it starts drawing again, is known as a "Vertical Blank Interval". This is usually the best tme to replace the Surface image.
+ 🚨 **`VK_PRESENT_MODE_IMMEDIATE_KHR`**
    + In this mode, the surface image will be replaced *immediately* after the next image to be displayed has finished rendering.
    + This can be an issue! Our Surface may only be halfway through drawing to the screen when we replace it, causing the second half to be a different image. This results in tearing.
+ 🚨 **`VK_PRESENT_MODE_FIFO_RELAXED_KHR`**
    + This mode works the same as FIFO mode, except for the situation where there are no images on the queue during a Vertical Blank.
    + If the presnetaiton queue is empty during a Vertical Blank it will start acting like Immediat mode.
    + The next image placed on to the presentation queue will no longer wait for a Vertical Blank. It will instead be passed straight to the surface, like in Immediat mode. This will cause tearing.
    + After it returns to FIFO
+ ✅ **`VK_PRESENT_MODE_MAILBOX_KHR`**
    + **Triple Buffer**
    + Images ready to present are added to a queue of size 1. The Surface uses this image only at a Vertical Blank to avoir tearing.
    + If a new image is sent to the queue to be presented, whatever is currently in the queue is discarded and becomes a vacant image ready to be drawn again.        
+ ✅ **`VK_PRESENT_MODE_FIFO_KHR`**
    + Images ready to present are added to a queue of a certain size. They are removed one at a time at each Vertical Blank, so there is no tearing.
    + Is a new image is sent to the queue to be presented, but the queue is full, the program will wait until it can add the image.
    + If the queue is empty when the next Vertical Blank occurs, it will redraw the image currently heald and check again at the next Vertical Blank.

#### Surface

+ Will act as an interface between the window created by GLFW and a Vulkan defined image in the Swapchain.
+ Surface will need to be created specifically for the window of the system we're using.
+ Could define this manually, but would make things complex when going cross-platform!
+ Fortunately, GLFW has a built-in function that returns an adequate surface for the current system being used.
    + `glfwCreateSurface();`

#### The "Presentation Queue"

+ In order to present a new Swapchain image to our surface, we need a queue that can handle present operations.
+ This is not really a type of queue, but a feature that some have.
+ Graphics queues usually have the required feature.
+ This means our Graphics queue and Presentation queue will usually be the same queue !

#### Images and Image Views

+ When the Swapchain is created, it will automatically create a set of imaes to be used by the Swapchain.
+ Since we will be drawing to them, we need access to them. To do this, the Swapchain can be querid and an array of the images can be returned. 
+ However these images can't be used directly since they are just the raw image.
+ An "ImageView" type can be created to interface with an image.
+ An ImageView simply describes how to read an image (e.g. 2D or 3D addresses, format, etc...) and what part of the image to read (colour channels, mip levels, etc...).
+ Later on, we will use these ImageViews when drawing to and presenting our images.

## Graphics Pipeline

> Setting up shaders and the Graphics Pipeline

### Vulkan Graphics Pipeline

+ The Graphics Pipeline in Vulkan is largely the sameas that in OpenGL.
+ However, in Vulkan you must setup each stage individually.
+ There is a lot of code to setup the Pipeline !
+ But it's mostly just applying setting to info structs. Nothing complicated.
+ Each Pipeline can connect to a "Render Pass" which actually handles rendering. This is the difficult part !

### Shaders and SPIR-V

+ Don't load in raw text and compile it into a shader
+ Instead, pre-compile shader code to intermediate code called SPIR-V, and load it into a shader module.
+ SPIR-V stands for "Standard Portable Intermediate Reprensentation - Vulkan"
+ Compiled from GLSL
+ SPIR-V can be compiled using tool that comes with LunarG Vulkan SDK.
+ To use: Load in .spv (SPIR-V file) as binary file and create shader module using Vulkan.
+ Shader module then passed to info struct.
+ Put all shader info structs into list and use in Pipeline Create Info.

### Creating Graphics Pipeline

+ Creating Graphics Pipeline is simple but lengthy. Must define settings for each manually
    + **Vertex Input**: Defines layout and format of vertex input data.
    + **Input Assembly**: Defines how to assemble vertices to primitives (e.g. Trianles or Lines).
    + **Viewport & Scissor**: How to fit output to image and crop it.
    + **Dynamic States**: Pipelines are static and settings can't be changed at runtime. You need to create a new pipeline to get new settings. However, some settings can be given ability to change at runtime, and they can be set there.
    + **Rasterizer**: How to handle the computation of fragments from primitives.
    + **Multisampling**: Multisampling information (e.g. for antialiasing)
    + **Blending**: How to blend framgents at the end of the pipeline.
    + **Depth Stencil**: How to determine depth + stencil culling and writing.

### Pipeline Layout

+ Pipeline Layout is the layout of data being given directly to the pipeline for a single draw operation (as opposed to for each vertex / fragment).
+ Defines layout of data for "Descriptor Sets". We'll learn about these later. They're similar to uniform buffers in OpenGL.
+ Also defines range of "Push Constants". These are like simpler, smaller Descriptor Sets that pass values directly instead of holding them in dedicated memory.
+ For now, we will leave these blanks

### Render Pass

+ Lastly a Pipeline needs a "Render Pass".
+ A Render Pass is the larger operation that handles the execution and outputs of a Pipeline.
+ Can have multiple smaller Subpasses inside it that each use a different Pipeline, so you can combine draws together.
+ Render Passes contain multiple "Attachments" to all of these possible outputs (e.g. Color Output Depth output...)
+ Subpasses inside a Render Pass then reference these attachments for each draw operation.

### Subpass Dependencies

+ Subpasses rely on strict ordering to ensure data is in the right format at the right time.
+ For example: Our Swapchain image being written to will need to be in a writable format at the stage of the subpass that writes to the image (Logical !)
+ However when we present it, it will need to be in a presentable format.
+ Subpass Dependencies define stages in a pipeline where transitions need to occur.
+ These are "Implicit" transitions since we do not explicitly state the transition that needs to take place. We only say when a transition should occur.
+ Vulkan knows what transitions to make because we define the layout at each stage of the subpass when we set it up.

## Color Spaces

> [Video](https://www.youtube.com/watch?v=99v96TL-tuY)

### What is a Color Space?

+ A color space defines **how color values (e.g. RGB)** should be interpreted to produce visible color.
+ Two images with the same RGB values can appear different if they use different color spaces.
+ It's a **mapping** from color values to human-visible colors.
+ Common color spaces include:
    + **sRGB** – Standard RGB, commonly used in monitors and web content.
    + **Linear** – Color values are stored in linear space, better for lighting calculations.
    + **Adobe RGB**, **Rec.709**, **Rec.2020**, etc. – Used in photography, video, and HDR.

### Why Does It Matter in Vulkan?

+ Vulkan separates the **color format** (e.g. `VK_FORMAT_B8G8R8A8_SRGB`) from the **color space** (e.g. `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR`).
+ The swapchain format describes **how images are stored**, while the color space tells Vulkan **how to interpret them** when presenting.

### Color Space in the Swapchain

+ When choosing a **surface format** for the swapchain, Vulkan will return a list of supported `(format, colorSpace)` pairs.
+ You must pick one that matches both your intended image format and the display's expected color space.
+ Most common and default option is:
    + `VK_FORMAT_B8G8R8A8_SRGB` + `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR`
    + This means: use **8-bit SRGB** and apply **sRGB gamma correction** automatically when presenting.

### Linear vs Nonlinear

+ **Linear space**:
    + Color values are proportional to actual light intensity.
    + Ideal for lighting and blending operations in shaders.
+ **Nonlinear space**:
    + Human vision is more sensitive to dark tones than bright ones.
    + sRGB applies a nonlinear curve (gamma) to store more detail in darker areas.
+ Vulkan allows you to **write shaders in linear space** and let the GPU **convert to nonlinear sRGB** during presentation.

### Practical Advice

+ Always check the list of supported surface formats and pick one that matches your needs.
+ If you want to perform accurate lighting in shaders, use **linear color in shaders**, then output to an **sRGB framebuffer**.
+ Avoid mismatching format and color space:
    + Using `VK_FORMAT_R8G8B8A8_UNORM` with `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR` may produce incorrect output.

