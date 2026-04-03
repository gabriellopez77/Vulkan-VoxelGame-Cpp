#include "DescriptorSet.h"

#include <cassert>
#include <memory>

#include <vulkan/vulkan.h>

#include "core/VulkanApp.h"
#include "Ubo.h"
#include "Texture.h"


void rk::DescriptorSet::create() {
    auto logicalDevice = VulkanApp::get()->getLogicalDevice();

    std::vector<VkDescriptorSetLayoutBinding> layoutsBinding;
    layoutsBinding.reserve(m_samplerLayouts.size() + m_uboLayouts.size());
    
    for (const auto& layoutInfo : m_uboLayouts) {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = layoutInfo.binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = (VkDescriptorType)layoutInfo.type;
        layoutBinding.stageFlags = (u32)layoutInfo.shaderStage;

        layoutsBinding.push_back(layoutBinding);
    }

    for (const auto& layoutInfo : m_samplerLayouts) {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = layoutInfo.binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = (VkDescriptorType)layoutInfo.type;
        layoutBinding.stageFlags = (u32)layoutInfo.shaderStage;

        layoutsBinding.push_back(layoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutsBinding.size();
    layoutInfo.pBindings = layoutsBinding.data();

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

    if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, m_descriptorSets) != VK_SUCCESS)
        assert(false && "failed to allocate descriptor set!");


    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::vector<std::unique_ptr<VkDescriptorBufferInfo>> buffersInfo;
    std::vector<std::unique_ptr<VkDescriptorImageInfo>> imagesInfo;

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        // ubo writes
        for (const auto& info : m_uboLayouts) {
            // create in heap because it is passed by reference
            auto bufferInfo = std::make_unique<VkDescriptorBufferInfo>();
            bufferInfo->buffer = info.ubo->getBuffer(i);
            bufferInfo->offset = 0;
            bufferInfo->range = info.ubo->getSize();

            VkWriteDescriptorSet writer{};
            writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writer.dstSet = m_descriptorSets[i];
            writer.dstBinding = info.binding;
            writer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writer.descriptorCount = 1;
            writer.pBufferInfo = bufferInfo.get();

            descriptorWrites.push_back(writer);
            buffersInfo.push_back(std::move(bufferInfo));
        }
        
        // sampler writes
        for (const auto& info : m_samplerLayouts) {
            // create in heap because it is passed by reference
            auto imageInfo = std::make_unique<VkDescriptorImageInfo>();
            imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo->imageView = info.texture->getImageView();
            imageInfo->sampler = info.texture->getSampler();

            VkWriteDescriptorSet writer{};
            writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writer.dstSet = m_descriptorSets[i];
            writer.dstBinding = info.binding;
            writer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writer.descriptorCount = 1;
            writer.pImageInfo = imageInfo.get();

            descriptorWrites.push_back(writer);
            imagesInfo.push_back(std::move(imageInfo));
        }

        vkUpdateDescriptorSets(logicalDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);

        // clean for the next loop iteration
        descriptorWrites.clear();
        buffersInfo.clear();
        imagesInfo.clear();
    }
}

void rk::DescriptorSet::addSampler(const Texture& texture, u32 binding, ShaderStage stage) {
    SamplerInfo layout{};
    layout.texture = &texture;
    layout.binding = binding;
    layout.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout.shaderStage = stage;

    m_samplerLayouts.push_back(layout);
}

void rk::DescriptorSet::addUbo(const Ubo& ubo, u32 binding, ShaderStage stage) {
    UboInfo layout{};
    layout.ubo = &ubo;
    layout.binding = binding;
    layout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout.shaderStage = stage;

    m_uboLayouts.push_back(layout);
}

void rk::DescriptorSet::bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout) const {
    vkCmdBindDescriptorSets(command, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
        &m_descriptorSets[VulkanApp::currentFrame()], 0, nullptr);
}
