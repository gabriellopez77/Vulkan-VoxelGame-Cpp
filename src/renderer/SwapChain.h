#pragma once

#include <vulkan/vulkan.h>
#include "VulkanFwd.h"
#include <array>

#include "Defs.h"
#include "Utils.h"


struct GLFWwindow;

namespace rk {
    // fwd
    class VulkanApp;
    class RenderPass;
    class LogicalDevice;

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
        void recreate(const VulkanApp* app, VkFence fence);

        u32 getWidth() const { return m_screenSize.width; }
        u32 getHeight() const { return m_screenSize.height; }
        VkExtent2D getSize() const { return m_screenSize; }

        u32 getImageIndex() const { return m_currentImageIndex; }

        VkFramebuffer getFramebuffer(u32 index) const { return m_framebuffers[index]; }
        VkFormat getImageFormat() const { return m_swapChainImageFormat; }
        VkSurfaceKHR getSurface() const { return m_surface; }
        VkSwapchainKHR& getSwapChain() { return m_swapChain; }

        u32 getOneImage(VkDevice device, VkSemaphore semaphore);

        void createSurface(const VulkanApp* app, GLFWwindow* window);
        SupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

    private:
        void createImageViews(const VulkanApp* app);

        u32 m_currentImageIndex = 0;

        VkExtent2D m_screenSize = {};
        VkSurfaceKHR m_surface = nullptr;
        VkSwapchainKHR m_swapChain = nullptr;
        VkFormat m_swapChainImageFormat = {};

        std::array<VkImage, utl::FRAMES_COUNT> m_images{};
        std::array<VkImageView, utl::FRAMES_COUNT> m_imageViews{};
        std::array<VkFramebuffer, utl::FRAMES_COUNT> m_framebuffers{};
    };
}