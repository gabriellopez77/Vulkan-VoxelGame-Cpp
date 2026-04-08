#pragma once

#include "Defs.h"
#include "VulkanFwd.h"
#include "render/VulkanEnums.h"


namespace rk::swapChain {
    extern bool isAdequate(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR surface);

    extern void create();
    extern void clear();
    extern void createFramebuffers();
    extern void recreate(VkFence fence, i32 width, i32 height);

    extern u32 getWidth();
    extern u32 getHeight();
    extern Extent2D getSize();

    extern VkFramebuffer getFramebuffer(u32 index);
    extern Formats getImageFormat();
    extern VkSurfaceKHR getSurface();
    extern VkSwapchainKHR& get();

    extern u32 getOneImage(VkDevice device, VkSemaphore semaphore);

    extern void setSurface(VkSurfaceKHR surface);
}