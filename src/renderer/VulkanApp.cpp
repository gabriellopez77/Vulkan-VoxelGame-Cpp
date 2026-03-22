#include "VulkanApp.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <fstream>
#include <cassert>
#include <iostream>

#include "PhysicalDevice.h"
#include "SwapChain.h"
#include "RenderPass.h"
#include "GraphicsPipeline.h"
#include "LogicalDevice.h"
#include "RenderUtils.h"


void VulkanApp::init(GLFWwindow* window) {
    this->window = window;
    physicalDevice = new PhysicalDevice();
    swapChain = new SwapChain();
    renderPass = new RenderPass();
    graphicsPipeline1 = new GraphicsPipeline();
    graphicsPipeline2 = new GraphicsPipeline();
    logicalDevice = new LogicalDevice();

    createInstance();
    swapChain->createSurface(this, window);
    physicalDevice->init(this); // pick physical device
    logicalDevice->init(this); // create logical device
    swapChain->init(this); // create swapChain
    swapChain->createImageViews(this);
    renderPass->init(this); // create renderPass
    graphicsPipeline1->init(this, PROJECT_ROOT_PATH"/assets/shaders/compiled/vertex1.spv", PROJECT_ROOT_PATH"/assets/shaders/compiled/fragment1.spv"); // create graphics pipeline
    graphicsPipeline2->init(this, PROJECT_ROOT_PATH"/assets/shaders/compiled/vertex2.spv", PROJECT_ROOT_PATH"/assets/shaders/compiled/fragment2.spv"); // create graphics pipeline
    swapChain->createFramebuffers(this); // create framebuffers
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();
}

void VulkanApp::clear() {
    vkDeviceWaitIdle(logicalDevice->get());

    swapChain->clear(this);
}

void VulkanApp::resize() {
    swapChain->resized = true;
}

void VulkanApp::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;


    // get required extensions
    auto requiredExtensions = renderUtils::getRequiredInstanceExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();


    // validation layer
    if (VALIDATION_LAYERS_ENABLED) {
        createInfo.enabledLayerCount = std::size(renderUtils::validateLayerNames);
        createInfo.ppEnabledLayerNames = renderUtils::validateLayerNames;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        assert(false && "Failed to create Vulkan instance!");
    }
}

VulkanApp::QueueFamilyIndices VulkanApp::findQueueFamilies(VkPhysicalDevice device) const {
    // get queue family properties count
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    // get queue family properties
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());


    // try to find a queue family that supports graphics
    QueueFamilyIndices indices;
    for (int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        // check if the queue family supports presentation to our window surface
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, swapChain->surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
            break;
    }

    return indices;
}

void VulkanApp::createCommandPool() {
    auto queueFamilyIndices = findQueueFamilies(physicalDevice->get());

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice->get(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");
}

void VulkanApp::createCommandBuffers() {
    m_commandBuffers.resize(swapChain->getImagesCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = swapChain->getImagesCount();

    if (vkAllocateCommandBuffers(logicalDevice->get(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        assert(false && "failed to allocate command buffers!");
}

void VulkanApp::createSyncObjects() {
    m_imageAvailableSemaphore.resize(swapChain->getImagesCount());
    m_renderFinishedSemaphore.resize(swapChain->getImagesCount());
    m_inFlightFence.resize(swapChain->getImagesCount());

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < swapChain->getImagesCount(); i++) {
        if (vkCreateSemaphore(logicalDevice->get(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice->get(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice->get(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS) {
            assert(false && "failed to create semaphores!");
        }
    }
}

void VulkanApp::recordCommandBuffer(u32 imageIndex) const {
    constexpr VkClearValue clearColor = { .color = { .float32 = { 0.f, 0.f, 0.f, 1.f } } };

    auto commandBuffer = m_commandBuffers[m_currentFrame];

    // rest command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        assert(false && "failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass->get();
    renderPassInfo.framebuffer = swapChain->getFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->getSize();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set dynamic states
    VkViewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = swapChain->getWidth();
    viewport.height = swapChain->getHeight();
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChain->getSize();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    // bind pipeline 1
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline2->get());
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);

    // bind pipeline 2
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline1->get());
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        assert(false && "failed to record command buffer!");
}

void VulkanApp::drawFrame() {
    // wait for previous frame
    vkWaitForFences(logicalDevice->get(), 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);

    // get one image from swap chain
    u32 imageIndex;
    auto result = swapChain->getOneImage(this, m_imageAvailableSemaphore[m_currentFrame], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapChain->recreate(this);
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        assert(false && "failed to acquire swap chain image!");
    }

    // set default fences state
    vkResetFences(logicalDevice->get(), 1, &m_inFlightFence[m_currentFrame]);

    // record draw infos in commandBuffer
    recordCommandBuffer(imageIndex);


    const VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphore[m_currentFrame] };
    const VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphore[m_currentFrame] };
    const VkSwapchainKHR swapChains[] = { swapChain->swapChain };
    const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(logicalDevice->getGraphicsQueue(), 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
        assert(false && "failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(logicalDevice->getPresentQueue(), &presentInfo);

    // recreate swap chain if necessary
    swapChain->tryRecreate(this, result, &m_inFlightFence[m_currentFrame]);

    //advance to the next frame
    m_currentFrame = (m_currentFrame + 1) % swapChain->getImagesCount();
}