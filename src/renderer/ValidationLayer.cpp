#include "ValidationLayer.h"

#include <Vulkan/Vulkan.h>

#include <vector>

#include <cstring>

#include "RenderUtils.h"
#include "VulkanApp.h"


void ValidationLayer::init(const VulkanApp* app) {

}

bool ValidationLayer::checkSupport() {;
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


    for (const char* layerName : renderUtils::validateLayerNames) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return false;
}
