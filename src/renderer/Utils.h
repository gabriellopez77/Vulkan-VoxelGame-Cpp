#pragma once

#include <vector>

#include "Defs.h"
#include "VulkanFwd.h"

#define SHADERS_FOLDER PROJECT_ROOT_PATH"/assets/shaders/compiled"


namespace rk {
    // fwd
    class VulkanApp;
}

namespace rk::utl {
    constexpr bool VALIDATION_LAYERS_ENABLED = true;
    constexpr i32 FRAMES_COUNT = 3;

    constexpr const char* validateLayerNames[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    // device extensions required by this application
    constexpr const char* deviceExtensions[] = {
        "VK_KHR_swapchain" // VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    // instance extensions required by this application
    extern std::vector<const char*> getRequiredInstanceExtensions();

    extern std::vector<char> readShaderFile(const char* filePath);

    extern u32 findMemoryType(const VulkanApp* app, u32 type, u32 properties);
    extern void createBuffer(const VulkanApp* app, u64 size, VkBuffer* buffer, VkDeviceMemory* memory, u32 usage, u32 properties);
    extern void copyBuffer(const VulkanApp* app, VkBuffer src, VkBuffer dst, u64 size);
}
