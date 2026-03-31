#include "DescriptorSetLayout.h"

#include <cassert>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"
#include "Ubo.h"


void rk::DescriptorSetLayout::create(u32 binding, ShaderStage stage) {
    auto logicalDevice = VulkanApp::get()->logicalDevice.get();

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = binding;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = (i32)stage;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
        assert(false && "failed to create descriptor set layout!");

    VkDescriptorSetLayout layouts[utl::FRAMES_COUNT];

    for (auto& layout : layouts)
        layout = m_descriptorSetLayout;

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = VulkanApp::get()->getDescriptorPool();
    allocInfo.descriptorSetCount = utl::FRAMES_COUNT;
    allocInfo.pSetLayouts = layouts;

    vkAllocateDescriptorSets(logicalDevice, &allocInfo, m_descriptorSets);
}

void rk::DescriptorSetLayout::setUbo(const Ubo& ubo, u32 binding) {
    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = ubo.getBuffer(i);
        bufferInfo.offset = 0;
        bufferInfo.range = ubo.getSize();

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_descriptorSets[i];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(VulkanApp::get()->logicalDevice.get(), 1, &descriptorWrite, 0, nullptr);
    }
}

void rk::DescriptorSetLayout::bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout) {
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
        &m_descriptorSets[VulkanApp::get()->getCurrentFrame()], 0, nullptr);
}
