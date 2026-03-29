#include "DescriptorSetLayout.h"

#include <cassert>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"
#include "Ubo.h"


void rk::DescriptorSetLayout::create(const VulkanApp* app, u32 binding, u32 stage) {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = binding;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = stage;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(app->logicalDevice.get(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
        assert(false && "failed to create descriptor set layout!");

    VkDescriptorSetLayout layouts[utl::FRAMES_COUNT];

    for (auto& l : layouts)
        l = m_descriptorSetLayout;

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = app->getDescriptorPool();
    allocInfo.descriptorSetCount = utl::FRAMES_COUNT;
    allocInfo.pSetLayouts = layouts;

    vkAllocateDescriptorSets(app->logicalDevice.get(), &allocInfo, m_descriptorSets);
}

void rk::DescriptorSetLayout::setUbo(const VulkanApp* app, const Ubo& ubo, u32 binding) {
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

        vkUpdateDescriptorSets(app->logicalDevice.get(), 1, &descriptorWrite, 0, nullptr);
    }
}

void rk::DescriptorSetLayout::bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout, u32 currentFrame) {
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
        &m_descriptorSets[currentFrame], 0, nullptr);
}
