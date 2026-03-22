#pragma once


// fwd
class VulkanApp;
class SwapChain;
typedef struct VkPhysicalDevice_T *VkPhysicalDevice;

class PhysicalDevice {
public:
    void init(const VulkanApp* app);
    bool checkExtensionSupport(VkPhysicalDevice device, const VulkanApp* app) const;

    VkPhysicalDevice get() const { return m_physicalDevice; }

private:

    bool isSuitable(VkPhysicalDevice device, const VulkanApp* app) const;

    VkPhysicalDevice m_physicalDevice = nullptr;
};

