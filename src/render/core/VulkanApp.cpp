#include "VulkanApp.h"

#include <cassert>
#include <iostream>
#include <cstring>
#include <set>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "Application.h"
#include "Utils.h"
#include "../PipelineSettings.h"
#include "../VulkanEnums.h"


rk::VulkanApp* rk::VulkanApp::m_instance = nullptr;
u32 rk::VulkanApp::m_lastFrame = 0;
u32 rk::VulkanApp::m_currentFrame = 0;
u32 rk::VulkanApp::m_currentImage = 0;

void rk::VulkanApp::init(const Application* application) {
    auto instance = new VulkanApp();
    m_instance = instance;

    instance->application = application;

    instance->createInstance();

    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance->m_vkInstance, application->getWindow(), nullptr, &surface) != VK_SUCCESS)
        assert(false && "failed to create window surface!");

    instance->swapChain.setSurface(surface);
    instance->pickPhysicalDevice();
    instance->createLogicalDevice();
    instance->swapChain.create(instance);
    instance->createRenderPass();
    instance->swapChain.createFramebuffers(instance);
    instance->createCommandPool();
    instance->createCommandBuffers();
    instance->createSyncObjects();
    instance->createDescriptorPool(10, 10);
}

void rk::VulkanApp::clear() {
    vkDeviceWaitIdle(m_logicalDevice);

    swapChain.clear(m_instance);
}

void rk::VulkanApp::resize() {
    int width, height;
    glfwGetFramebufferSize(application->getWindow(), &width, &height);

    // avoid create a framebuffer with 0 as size
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(application->getWindow(), &width, &height);
        glfwWaitEvents();
    }

    swapChain.recreate(m_instance, m_inFlightFence[m_lastFrame], width, height);

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
    std::vector<const char*> requiredExtensions;

    u32 glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    requiredExtensions.reserve(glfwExtensionCount);

    for (int i = 0; i < glfwExtensionCount; i++)
        requiredExtensions.push_back(glfwExtensions[i]);

    if (utl::VALIDATION_LAYERS_ENABLED)
        requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


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

void rk::VulkanApp::pickPhysicalDevice() {
    auto isSuitable = [](VkPhysicalDevice device, const VulkanApp* app) -> bool {
        auto checkExtensionSupport = [](VkPhysicalDevice device) {
            // get device extension count
            u32 extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            // get device extensions
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            for (auto extensionName : rk::utl::deviceExtensions) {
                bool supported = false;

                for (auto extensionProperties : availableExtensions)
                    if (strcmp(extensionName, extensionProperties.extensionName))
                        supported = true;

                if (!supported)
                    return false;
            }

            return true;
        };

        // check if the device has a graphics queue family
        auto indices = app->findQueueFamilies(device);

        // check if device supports all required extensions
        bool extensionsSupported = checkExtensionSupport(device);

        bool swapChainAdequate = false;

        // if extensions are supported, check if the swap chain is adequate
        if (extensionsSupported) {
            swapChainAdequate = SwapChain::isAdequate(device, app->swapChain.getSurface());;
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    };

    // get physical devices count
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

    if (deviceCount == 0)
        assert(false && "Failed to find GPUs with Vulkan support!");

    // get physical devices
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

    // check each device for suitability and choice the first one that is suitable
    for (int i = 0; i < deviceCount; i++) {
        if (isSuitable(devices[i], this)) {
            m_physicalDevice = devices[i];
            break;
        }
    }

    if (m_physicalDevice == nullptr)
        assert(false && "Failed to find a suitable GPU!");
}

void rk::VulkanApp::createLogicalDevice() {
    // grab queue family indices
    auto indices = findQueueFamilies();

    // create device queue create info
    const f32 queuePriority = 1.0f;

    // we need to create a queue for each unique queue family (we use a std::set why graphics and present can have the same index)
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<u32> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value(), indices.transferFamily.value() };
    for (u32 queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    // create logical device create info
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.enabledLayerCount = 0;
    createInfo.enabledExtensionCount = std::size(utl::deviceExtensions);
    createInfo.ppEnabledExtensionNames = utl::deviceExtensions;

    // create the logical device
    if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_logicalDevice) != VK_SUCCESS)
        assert(false && "failed to create logical device!");

    // get graphics and present queues
    vkGetDeviceQueue(m_logicalDevice, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_logicalDevice, indices.presentFamily.value(), 0, &m_presentQueue);
    vkGetDeviceQueue(m_logicalDevice, indices.transferFamily.value(), 0, &m_transferQueue);
}

void rk::VulkanApp::createRenderPass() {
        // color attachment
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[2] = {};

    // color
    attachments[0].format = (VkFormat)swapChain.getImageFormat();
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // depth
    attachments[1].format = (VkFormat)Formats::DEPTH_F32;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = std::size(attachments);
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_logicalDevice, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        assert(false && "failed to create render pass!");
}

rk::VulkanApp::QueueFamilyIndices rk::VulkanApp::findQueueFamilies(VkPhysicalDevice device) const {
    auto physicalDevice = device == nullptr ? m_physicalDevice : device;

    // get queue family properties count
    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    // get queue family properties
    std::vector<VkQueueFamilyProperties> queues(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queues.data());


    QueueFamilyIndices indices;

    // graphics
    for (int i = 0; i < queues.size(); i++) {
        u32 flags = queues[i].queueFlags;

        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            break;
        }
    }

    assert(indices.graphicsFamily.has_value());

    // present
    for (int i = 0; i < queues.size(); i++) {
        u32 flags = queues[i].queueFlags;

        // try to get present queue that not contains graphics bit
        if ((flags & VK_QUEUE_GRAPHICS_BIT) == 0) {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, swapChain.getSurface(), &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
                break;
            }
        }
    }
    if (!indices.presentFamily.has_value())
        indices.presentFamily = indices.graphicsFamily;

    // transfer
    for (int i = 0; i < queues.size(); i++) {
        u32 flags = queues[i].queueFlags;

        // try to get transfer queue that not contains graphics bit
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

    if (vkCreateCommandPool(m_logicalDevice, &poolInfo, nullptr, &m_transferCommandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");

    VkCommandPoolCreateInfo poolInfo2{};
    poolInfo2.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo2.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo2.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(m_logicalDevice, &poolInfo2, nullptr, &m_graphicsCommandPool) != VK_SUCCESS)
        assert(false && "failed to create command pool!");
}

void rk::VulkanApp::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_graphicsCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = utl::FRAMES_COUNT;


    if (vkAllocateCommandBuffers(m_logicalDevice, &allocInfo, m_commandBuffers) != VK_SUCCESS)
        assert(false && "failed to allocate command buffers!");
}

void rk::VulkanApp::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        if (vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_logicalDevice, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore[i]) != VK_SUCCESS ||
            vkCreateFence(m_logicalDevice, &fenceInfo, nullptr, &m_inFlightFence[i]) != VK_SUCCESS) {
            assert(false && "failed to create semaphores!");
        }
    }
}

void rk::VulkanApp::createDescriptorPool(u32 maxSetsInDescriptors, u32 maxDescriptorCount) {
    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, utl::FRAMES_COUNT * maxDescriptorCount },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, utl::FRAMES_COUNT * maxDescriptorCount },
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = std::size(poolSizes);
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = maxSetsInDescriptors;

    if (vkCreateDescriptorPool(m_logicalDevice, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
        assert(false && "failed to create descriptor pool!");
}

void rk::VulkanApp::beginFrame() {
    // wait for previous frame
    vkWaitForFences(m_logicalDevice, 1, &m_inFlightFence[m_currentFrame], VK_TRUE, UINT64_MAX);

    // get next image from swapChain
    u32 imageIndex = swapChain.getOneImage(m_logicalDevice, m_imageAvailableSemaphore[m_currentFrame]);

    if (m_imagesInFlight[imageIndex] != nullptr)
        vkWaitForFences(m_logicalDevice, 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);

    m_imagesInFlight[swapChain.getImageIndex()] = m_inFlightFence[m_currentFrame];

    // reset fence state
    vkResetFences(m_logicalDevice, 1, &m_inFlightFence[m_currentFrame]);


    auto commandBuffer = m_commandBuffers[m_currentFrame];

    // reset command buffer
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        assert(false && "failed to begin recording command buffer!");

    const VkClearValue clearValues[2] = {
        { .color = {{0.0f, 0.0f, 0.0f, 1.0f}} },
        { .depthStencil = {1.0f, 0} }
    };

    VkExtent2D extent = { swapChain.getWidth(), swapChain.getHeight() };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = swapChain.getFramebuffer(imageIndex);
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.clearValueCount = std::size(clearValues);
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // set dynamic states
    VkRect2D scissor{};
    scissor.extent = extent;

    VkViewport viewport{};
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
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

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFence[m_currentFrame]) != VK_SUCCESS)
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

    vkQueuePresentKHR(m_presentQueue, &presentInfo);

    //advance to the next frame
    m_lastFrame = m_currentFrame;
    m_currentFrame = (m_currentFrame + 1) % utl::FRAMES_COUNT;
}