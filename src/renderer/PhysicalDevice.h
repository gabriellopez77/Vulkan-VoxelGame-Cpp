#pragma once

#include "VulkanFwd.h"



namespace render {
    // fwd
    class VulkanApp;
    class SwapChain;
    
    class PhysicalDevice {
    public:
        void create(const VulkanApp* app);
        bool checkExtensionSupport(VkPhysicalDevice device, const VulkanApp* app) const;

        VkPhysicalDevice get() const { return m_physicalDevice; }

    private:

        bool isSuitable(VkPhysicalDevice device, const VulkanApp* app) const;

        VkPhysicalDevice m_physicalDevice = nullptr;
    };
}

