#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Defs.h"
#include "Utils.h"


struct GLFWwindow;

namespace rk {
    // fwd
    class VulkanApp;
    class RenderPass;
    class LogicalDevice;
    enum class Formats : i32;

    class SwapChain {
    public:
        struct SupportDetails {
            VkSurfaceCapabilitiesKHR capabilities{};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        void create(const VulkanApp* app);
        void clear(const VulkanApp* app) const;
        void createFramebuffers(const VulkanApp* app);
        VkExtent2D recreate(const VulkanApp* app, VkFence fence);

        u32 getWidth() const { return m_screenSize.width; }
        u32 getHeight() const { return m_screenSize.height; }
        VkExtent2D getSize() const { return m_screenSize; }

        u32 getImageIndex() const { return m_currentImageIndex; }

        VkFramebuffer getFramebuffer(u32 index) const { return m_framebuffers[index]; }
        Formats getImageFormat() const { return m_imagesFormat; }
        VkSurfaceKHR getSurface() const { return m_surface; }
        VkSwapchainKHR& get() { return m_swapChain; }

        u32 getOneImage(VkDevice device, VkSemaphore semaphore);

        void createSurface(const VulkanApp* app);
        SupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

    private:
        void createImageViews();

        u32 m_currentImageIndex = 0;

        VkExtent2D m_screenSize = {};
        VkSurfaceKHR m_surface = nullptr;
        VkSwapchainKHR m_swapChain = nullptr;
        Formats m_imagesFormat = {};

        // depth buffer
        VkImage m_depthImage = nullptr;
        VkDeviceMemory m_depthImageMemory = nullptr;
        VkImageView m_depthImageView = nullptr;

        VkImage m_images[utl::FRAMES_COUNT] = {};
        VkImageView m_imageViews[utl::FRAMES_COUNT] = {};
        VkFramebuffer m_framebuffers[utl::FRAMES_COUNT] = {};
    };
}