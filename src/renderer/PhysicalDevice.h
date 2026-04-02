#pragma once

#include "VulkanFwd.h"


namespace rk {
    // fwd
    class VulkanApp;
    class SwapChain;
    
    class PhysicalDevice {
    public:
        void create(const VulkanApp* app);

        VkPhysicalDevice get() const { return m_physicalDevice; }

    private:
        VkPhysicalDevice m_physicalDevice = nullptr;
    };
}

