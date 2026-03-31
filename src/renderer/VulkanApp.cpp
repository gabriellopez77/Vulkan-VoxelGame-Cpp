#include "VulkanApp.h"

#include <bitset>
#include <cassert>
#include <iostream>

#include <GLFW/glfw3.h>

#include "Utils.h"
#include "PipelineSettings.h"


rk::VulkanApp* rk::VulkanApp::m_instance = nullptr;

void rk::VulkanApp::init(GLFWwindow* window) {
    auto instance = new VulkanApp();
    m_instance = instance;

    instance->window = window;

    instance->createInstance();

    instance->swapChain.createSurface(instance);
    instance->physicalDevice.create(instance); // pick physical device
    instance->logicalDevice.create(instance); // create logical device
    instance->swapChain.create(instance); // create swapChain
    instance->renderPass.create(instance); // create renderPass
    instance->swapChain.createFramebuffers(instance);
    instance->createCommandPool();
    instance->createCommandBuffers();
    instance->createSyncObjects();
    instance->createDescriptorPool();

    instance->m_viewport.width = instance->swapChain.getWidth();
    instance->m_viewport.height = instance->swapChain.getHeight();

    instance->m_scissor.extent = instance->swapChain.getSize();
}

void rk::VulkanApp::clear() {
    vkDeviceWaitIdle(logicalDevice.get());

    swapChain.clear(m_instance);
}

void rk::VulkanApp::resize() {
    auto newSize = swapChain.recreate(m_instance, m_inFlightFence[m_lastFrame]);

    m_viewport.width = newSize.width;
    m_viewport.height = newSize.height;

    m_scissor.extent = newSize;

    for (auto& fence : m_imagesInFlight)
        fence = nullptr;
}

void rk::VulkanApp::createInstance() {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // get required extensions
    auto requiredExtensions = utl::getRequiredInstanceExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    // validation layer
    if (utl::VALIDATION_LAYERS_ENABLED) {
        createInfo.enabledLayerCount = std::size(utl::validateLayerNames);
        createInfo.ppEnabledLayerNames = utl::validateLayerNames;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
        assert(false && "Failed to create Vulkan instance!");
}

rk::VulkanApp::QueueFamilyIndices rk::VulkanApp::findQueueFamilies(VkPhysicalDevice device,
    const std::vector<VkQueueFamilyProperties>* queues) const {
    const auto& queuesFamilies = queues == nullptr ? *physicalDevice.getQueueFamiliesCache() : *queues;
    auto de = device == nullptr ? physicalDevice.get() : device;


    QueueFamilyIndices indices;

    // graphics
    for (int i = 0; i < queuesFamilies.size(); i++) {
        u32 flags = queuesFamilies[i].queueFlags;

        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            break;
        }
    }

    assert(indices.graphicsFamily.has_value());

    // present
    for (int i = 0; i < queuesFamilies.size(); i++) {
        u32 flags = queuesFamilies[i].queueFlags;

        // try to get present queue that not contains graphics bit
        if ((flags & VK_QUEUE_GRAPHICS_BIT) == 0) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(de, i, swapChain.getSurface(), &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
                break;
            }
        }
    }
    if (!indices.presentFamily.has_value())
        indices.presentFamily = indices.graphicsFamily;

    // transfer
    for (int i = 0; i < queuesFamilies.size(); i++) {
        u32 flags = queuesFamilies[i].queueFlags;

        if (flags & VK_QUEUE_TRANSFER_BIT && flags & VK_QUEUE_GRAPHICS_BIT == 0) {
            indices.transferFamily = i;
            break;
        }
    }
    if (!indices.transferFamily.has_value())
        indices.transferFamily = indices.graphicsFamily;

    return indices;
}

void rk::VulkanApp::createCommandPool() {
    auto queueFamilyIndices = findQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily.value();

    if (vkCreateCommandPool(logicalDevice.get(), &poolInfo, nullptr, &m_transferCommandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");

    VkCommandPoolCreateInfo poolInfo2{};
    poolInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo2.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo2.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(logicalDevice.get(), &poolInfo2, nullptr, &m_graphicsCommandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");
}

void rk::VulkanApp::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_graphicsCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = utl::FRAMES_COUNT;


    if (vkAllocateCommandBuffers(logicalDevice.get(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        assert(false && "failed to allocate command buffers!");
}

void rk::VulkanApp::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        if (vkCreateSemaphore(logicalDevice.get(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(logicalDevice.get(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(logicalDevice.get(), &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS) {
            assert(false && "failed to create semaphores!");
        }
    }
}

void rk::VulkanApp::createDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = utl::FRAMES_COUNT;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = utl::FRAMES_COUNT;

    if (vkCreateDescriptorPool(logicalDevice.get(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
        assert(false && "failed to create descriptor pool!");
}

void rk::VulkanApp::beginFrame() {
    // wait for previous frame
    vkWaitForFences(logicalDevice.get(), 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);

    // get next image from swapChain
    u32 imageIndex = swapChain.getOneImage(logicalDevice.get(), m_imageAvailableSemaphore[m_currentFrame]);

    if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(logicalDevice.get(), 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);

    m_imagesInFlight[swapChain.getImageIndex()] = m_inFlightFence[m_currentFrame];

    // reset fence state
    vkResetFences(logicalDevice.get(), 1, &m_inFlightFence[m_currentFrame]);


    auto commandBuffer = m_commandBuffers[m_currentFrame];

    // reset command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        assert(false && "failed to begin recording command buffer!");

    constexpr VkClearValue clearColor = { .color = { .float32 = { 0.f, 0.f, 0.f, 1.f } } };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass.get();
    renderPassInfo.framebuffer = swapChain.getFramebuffer(imageIndex);
    renderPassInfo.renderArea.extent = swapChain.getSize();
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


    // set dynamic states
    vkCmdSetViewport(commandBuffer, 0, 1, &m_viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &m_scissor);
}

void rk::VulkanApp::endFrame() {
    auto commandBuffer = m_commandBuffers[m_currentFrame];

    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        assert(false && "failed to record command buffer!");


    const auto signalSemaphore = m_renderFinishedSemaphore[m_currentFrame];
    constexpr VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &m_imageAvailableSemaphore[m_currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &signalSemaphore;

    if (vkQueueSubmit(logicalDevice.getGraphicsQueue(), 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
        assert(false && "failed to submit draw command buffer!");


    // the same image index obtained from getOneImage in begin frame
    u32 imageIndex = swapChain.getImageIndex();

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &signalSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain.get();
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(logicalDevice.getPresentQueue(), &presentInfo);

    //advance to the next frame
    m_lastFrame = m_currentFrame;
    m_currentFrame = (m_currentFrame + 1) % utl::FRAMES_COUNT;
}