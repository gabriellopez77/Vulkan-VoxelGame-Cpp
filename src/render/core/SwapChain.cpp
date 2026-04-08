#include "SwapChain.h"

#include <cassert>
#include <vector>

#include <vulkan/vulkan.h>

#include "Window.h"
#include "VulkanApp.h"

#include "math/Math.h"


struct SupportDetails {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
SupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);


namespace rk::swapChain {
    void createImageViews();

    Extent2D screenSize = {};
    VkSurfaceKHR surface = nullptr;
    VkSwapchainKHR vkSwapChain = nullptr;
    Formats imagesFormat = {};

    // depth buffer
    VkImage depthImage = nullptr;
    VkDeviceMemory depthImageMemory = nullptr;
    VkImageView depthImageView = nullptr;

    VkImage images[utl::FRAMES_COUNT] = {};
    VkImageView imageViews[utl::FRAMES_COUNT] = {};
    VkFramebuffer framebuffers[utl::FRAMES_COUNT] = {};

    u32 getWidth() { return screenSize.width; }
    u32 getHeight() { return screenSize.height; }
    Extent2D getSize() { return screenSize; }

    VkFramebuffer getFramebuffer(u32 index) { return framebuffers[index]; }
    Formats getImageFormat() { return imagesFormat; }
    VkSurfaceKHR getSurface() { return surface; }
    VkSwapchainKHR& get() { return vkSwapChain; }

    void setSurface(VkSurfaceKHR surface) { swapChain::surface = surface; };

    bool isAdequate(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
        auto swapChainSupport = querySwapChainSupport(physicalDevice, surface);
        return !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    void create() {
        auto logicalDevice = vulkanApp::getLogicalDevice();

        auto swapChainSupport = querySwapChainSupport(vulkanApp::getPhysicalDevice(), surface);

        // choose the best settings for our swap chain
        auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        auto extent = chooseSwapExtent(swapChainSupport.capabilities);

        assert(swapChainSupport.capabilities.minImageCount <= utl::FRAMES_COUNT && "swapChain images count not supported");


        // create swap chain create info
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = utl::FRAMES_COUNT;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto indices = vulkanApp::findQueueFamilies();
        u32 queueFamilyIndices[] = { indices.graphics.value(), indices.present.value() };

        if (indices.graphics != indices.present) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = nullptr;

        // create the swap chain
        if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &vkSwapChain) != VK_SUCCESS)
            assert(false && "failed to create swap chain!");


        // get swap chain images
        u32 imageCount = utl::FRAMES_COUNT;
        vkGetSwapchainImagesKHR(logicalDevice, vkSwapChain, &imageCount, images);

        imagesFormat = (Formats)surfaceFormat.format;
        screenSize = { extent.width, extent.height };

        createImageViews();

        utl::createImage(extent.width, extent.height, depthImage, depthImageMemory, Formats::DEPTH_F32,
            MemoryType::DEVICE_LOCAL, ImageUsage::DEPTH_STENCIL_ATTACHMENT);

        depthImageView = utl::createImageView(depthImage, Formats::DEPTH_F32, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void createImageViews() {
        for (size_t i = 0; i < utl::FRAMES_COUNT; i++)
            imageViews[i] = utl::createImageView(images[i], imagesFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void createFramebuffers() {
        for (int i = 0; i < utl::FRAMES_COUNT; i++) {
            VkImageView attachments[] = { imageViews[i], depthImageView };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = vulkanApp::getRenderPass();
            framebufferInfo.attachmentCount = std::size(attachments);
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = screenSize.width;
            framebufferInfo.height = screenSize.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(vulkanApp::getLogicalDevice(), &framebufferInfo, nullptr, &framebuffers[i]))
                assert(false && "failed to create framebuffer!");
        }
    }


    void clear() {
        auto logicalDevice = rk::vulkanApp::getLogicalDevice();

        for (int i = 0; i < utl::FRAMES_COUNT; i++) {
            vkDestroyFramebuffer(logicalDevice, framebuffers[i], nullptr);
            vkDestroyImageView(logicalDevice, imageViews[i], nullptr);
        }

        // depth buffer
        vkDestroyImageView(logicalDevice, depthImageView, nullptr);
        vkDestroyImage(logicalDevice, depthImage, nullptr);
        vkFreeMemory(logicalDevice, depthImageMemory, nullptr);

        vkDestroySwapchainKHR(logicalDevice, vkSwapChain, nullptr);
    }

    void recreate(VkFence fence, i32 width, i32 height) {
        // wait for command is finished
        vkWaitForFences(vulkanApp::getLogicalDevice(), 1, &fence, VK_TRUE, UINT64_MAX);

        // destroy the old swapChain
        clear();

        // create a new swapChain
        create();
        createFramebuffers();
    }

    u32 getOneImage(VkDevice device, VkSemaphore semaphore) {
        u32 imageIndex = 0;
        auto result = vkAcquireNextImageKHR(device, rk::swapChain::vkSwapChain, UINT64_MAX, semaphore, nullptr, &imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR)
            assert(false && "failed to acquire swap chain image!");

        return imageIndex;
    }
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return availableFormat;
    }

    // if the preferred format isn't available, just return the first one from the list
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    // try to choose VK_PRESENT_MODE_MAILBOX_KHR mode
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    // if the preferred present mode isn't available just return VK_PRESENT_MODE_FIFO_KHR (which is guaranteed to be available)
    //return VK_PRESENT_MODE_IMMEDIATE_KHR;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    int width, height;
    window::getFrameBufferSize(&width, &height);

    VkExtent2D actualExtent = { (u32)width, (u32)height };

    // limits extent between min and max extents supported by the surface
    actualExtent.width = math::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = math::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

SupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SupportDetails details;

    // get physical device surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // get physical device surface formats count
    u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    // get physical device surface present modes count
    u32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    // get physical device surface present modes
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}