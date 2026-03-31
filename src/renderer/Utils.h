#pragma once

#include <vector>

#include "Defs.h"
#include "VulkanFwd.h"

#define SHADERS_FOLDER PROJECT_ROOT_PATH"/assets/shaders/compiled"


namespace rk {
    // fwd
    class VulkanApp;
    enum class BufferUsage : u32;
    enum class MemoryType : u32;
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

    extern u32 findMemoryType(u32 type, MemoryType memoryType);

    extern void copyDataToStagingBuffer(u64 size, VkDeviceMemory memoryType, const void* data);

    extern void createBuffer(u64 size, VkBuffer& buffer, VkDeviceMemory& memory, BufferUsage usage, MemoryType memoryType);
    extern void copyBuffer(VkBuffer src, VkBuffer dst, u64 size);

    extern void createImage(u32 width, u32 height);
}
