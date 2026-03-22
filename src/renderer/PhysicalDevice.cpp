#include "PhysicalDevice.h"

#include <cassert>
#include <vector>
#include <cstring>
#include "../defs.h"

#include "VulkanApp.h"
#include "SwapChain.h"
#include "RenderUtils.h"


void PhysicalDevice::init(const VulkanApp* app) {
    // get physical devices count
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        assert(false && "Failed to find GPUs with Vulkan support!");
    }

    // get physical devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(app->instance, &deviceCount, devices.data());

    // check each device for suitability and choice the first one that is suitable
    for (int i = 0; i < deviceCount; i++) {
        if (isSuitable(devices[i], app)) {
            m_physicalDevice = devices[i];
            break;
        }
    }

    if (m_physicalDevice == nullptr) {
        assert(false && "Failed to find a suitable GPU!");
    }
}



bool PhysicalDevice::checkExtensionSupport(VkPhysicalDevice device, const VulkanApp* app) const {
    // get device extension count
    u32 extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    // get device extensions
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());



    for (auto extensionName : renderUtils::deviceExtensions) {
        bool supported = false;

        for (auto extensionProperties : availableExtensions)
            if (strcmp(extensionName, extensionProperties.extensionName))
                supported = true;

        if (!supported)
            return false;
    }

    // check if all required extensions are available
    //std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    // if extension is in deviceExtensions then remove it from requiredExtensions
    //for (const auto& extension : availableExtensions)
    //    requiredExtensions.erase(extension.extensionName);

    // if empty, all required extensions are supported
    //return requiredExtensions.empty();
    return true;
}

bool PhysicalDevice::isSuitable(VkPhysicalDevice device, const VulkanApp* app) const {
    // check if the device has a graphics queue family
    auto indices = app->findQueueFamilies(device);

    // check if device supports all required extensions
    bool extensionsSupported = checkExtensionSupport(device, app);

    bool swapChainAdequate = false;

    // if extensions are supported, check if the swap chain is adequate
    if (extensionsSupported) {
        auto swapChainSupport = app->swapChain->querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}
