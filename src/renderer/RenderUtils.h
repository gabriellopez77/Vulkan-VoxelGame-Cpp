#pragma once

#include <vector>


namespace renderUtils {
    constexpr const char* validateLayerNames[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    // device extensions required by this application
    constexpr const char* deviceExtensions[] = {
        "VK_KHR_swapchain" // VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    // instance extensions required by this application
    extern std::vector<const char*> getRequiredInstanceExtensions();
}