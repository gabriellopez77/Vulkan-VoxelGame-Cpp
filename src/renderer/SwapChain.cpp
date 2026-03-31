#include "SwapChain.h"

#include <vulkan/vulkan_core.h>
#include <glfw/glfw3.h>

#include <cassert>
#include <algorithm>

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "RenderPass.h"
#include "VulkanApp.h"
#include "Defs.h"


static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
static VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

void rk::SwapChain::create(const VulkanApp* app) {
    auto logicalDevice = VulkanApp::get()->logicalDevice.get();

    auto swapChainSupport = querySwapChainSupport(app->physicalDevice.get());

    // choose the best settings for our swap chain
    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(app->window, swapChainSupport.capabilities);

    assert(swapChainSupport.capabilities.minImageCount <= utl::FRAMES_COUNT && "swapChain images count not supported");

    // create swap chain create info
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = utl::FRAMES_COUNT;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = app->findQueueFamilies(app->physicalDevice.get());
    u32 queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
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
    if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
        assert(false && "failed to create swap chain!");


    u32 imageCount = utl::FRAMES_COUNT;

    // get swap chain images
    vkGetSwapchainImagesKHR(logicalDevice, m_swapChain, &imageCount, nullptr);
    vkGetSwapchainImagesKHR(logicalDevice, m_swapChain, &imageCount, m_images.data());

    m_swapChainImageFormat = surfaceFormat.format;
    m_screenSize = extent;


    createImageViews(app);
}

void rk::SwapChain::createImageViews(const VulkanApp* app) {
    for (size_t i = 0; i < utl::FRAMES_COUNT; i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = m_images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = m_swapChainImageFormat;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(app->logicalDevice.get(), &createInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
            assert(false && "failed to create image views!");
    }
}

void rk::SwapChain::createFramebuffers(const VulkanApp* app) {
    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = app->renderPass.get();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &m_imageViews[i];
        framebufferInfo.width = m_screenSize.width;
        framebufferInfo.height = m_screenSize.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(app->logicalDevice.get(), &framebufferInfo, nullptr, &m_framebuffers[i]))
            assert(false && "failed to create framebuffer!");
    }
}

void rk::SwapChain::createSurface(const VulkanApp* app) {
    if (glfwCreateWindowSurface(app->getVkInstance(), app->window, nullptr, &m_surface) != VK_SUCCESS)
        assert(false && "failed to create window surface!");
}

rk::SwapChain::SupportDetails rk::SwapChain::querySwapChainSupport(VkPhysicalDevice device) const {
    SupportDetails details;

    // get physical device surface capabilities
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    // get physical device surface formats count
    u32 formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    // get physical device surface present modes count
    u32 presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    // get physical device surface present modes
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

u32 rk::SwapChain::getOneImage(VkDevice device, VkSemaphore semaphore) {
    auto result = vkAcquireNextImageKHR(device, m_swapChain, UINT64_MAX, semaphore, nullptr, &m_currentImageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR) {
        assert(false && "failed to acquire swap chain image!");
    }

    return m_currentImageIndex;
}

void rk::SwapChain::clear(const VulkanApp* app) const {
    auto logicalDevice = app->logicalDevice.get();

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        vkDestroyFramebuffer(logicalDevice, m_framebuffers[i], nullptr);
        vkDestroyImageView(logicalDevice, m_imageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, m_swapChain, nullptr);
}

VkExtent2D rk::SwapChain::recreate(const VulkanApp* app, VkFence fence) {
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(app->window, &width, &height);

    // avoid create a framebuffer with 0 as size
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(app->window, &width, &height);
        glfwWaitEvents();
    }

    // wait for command is finished
    vkWaitForFences(app->logicalDevice.get(), 1, &fence, VK_TRUE, UINT64_MAX);

    // destroy the old swapChain
    clear(app);

    // create a new swapChain
    create(app);
    createFramebuffers(app);

    return { (u32)width, (u32)height };
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

VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = { (u32)width, (u32)height };

    // limits extent between min and max extents supported by the surface
    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}