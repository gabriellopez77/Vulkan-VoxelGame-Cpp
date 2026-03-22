#pragma once

#include "vulkan/vulkan.h"

#include <vector>

#include "../defs.h"


// fwd
struct GLFWwindow;
class VulkanApp;
class RenderPass;
class LogicalDevice;

class SwapChain {
public:
    struct SupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    void init(const VulkanApp* app);
    void clear(const VulkanApp* app);
    void recreate(const VulkanApp* app);

    u32 getWidth() const { return m_screenSize.width; }
    u32 getHeight() const { return m_screenSize.height; }
    VkExtent2D getSize() const { return m_screenSize; }

    u32 getImagesCount() const { return m_imagesCount; }

    void tryRecreate(const VulkanApp* app, VkResult, const VkFence* fence);

    VkFramebuffer getFramebuffer(u32 index) const { return m_framebuffers[index]; }
    void createSurface(const VulkanApp* app, GLFWwindow* window);
    void createImageViews(const VulkanApp* app);
    void createFramebuffers(const VulkanApp* app);
    SupportDetails querySwapChainSupport(VkPhysicalDevice device) const;
    VkResult getOneImage(const VulkanApp* logicalDevice, VkSemaphore semaphore, u32* imageIndex);

    VkSurfaceKHR surface = nullptr;
    VkSwapchainKHR swapChain = nullptr;
    VkFormat swapChainImageFormat = {};
    bool resized = false;

private:
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

    u32 m_imagesCount = 0;
    VkExtent2D m_screenSize = {};
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    std::vector<VkFramebuffer> m_framebuffers;
};
