#include "DescriptorSet.h"

#include <cassert>
#include <memory>

#include <vulkan/vulkan.h>

#include "VulkanApp.h"
#include "RawTexture.h"
#include "math/Math.h"


void rk::DescriptorSet::create() {
    auto logicalDevice = vulkanApp::getLogicalDevice();

    std::vector<VkDescriptorSetLayoutBinding> layoutsBinding;
    layoutsBinding.reserve(m_layouts.size());
    
    for (const auto& layoutInfo : m_layouts) {
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = layoutInfo.binding;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = static_cast<VkDescriptorType>(layoutInfo.type);
        layoutBinding.stageFlags = static_cast<u32>(layoutInfo.shaderStage);

        layoutsBinding.push_back(layoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<u32>(layoutsBinding.size());
    layoutInfo.pBindings = layoutsBinding.data();

    if (vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        assert(false && "failed to create descriptor set layout!");
    }

    VkDescriptorSetLayout layouts[utl::FRAMES_COUNT];

    for (auto& layout : layouts)
        layout = m_descriptorSetLayout;

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = vulkanApp::getDescriptorPool();
    allocInfo.descriptorSetCount = utl::FRAMES_COUNT;
    allocInfo.pSetLayouts = layouts;

    if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, m_descriptorSets) != VK_SUCCESS) {
        assert(false && "failed to allocate descriptor set!");
    }


    std::vector<VkWriteDescriptorSet> descriptorWrites;
    std::vector<std::unique_ptr<VkDescriptorBufferInfo>> buffersInfo;
    std::vector<std::unique_ptr<VkDescriptorImageInfo>> imagesInfo;

    for (i32 i = 0; i < utl::FRAMES_COUNT; i++) {
        for (const auto& info : m_layouts) {
            VkWriteDescriptorSet writer{};
            writer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writer.dstSet = m_descriptorSets[i];
            writer.dstBinding = info.binding;
            writer.descriptorType = static_cast<VkDescriptorType>(info.type);
            writer.descriptorCount = 1;

            if (info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) {
                // create in heap because it is passed by reference
                auto bufferInfo = std::make_unique<VkDescriptorBufferInfo>();
                bufferInfo->offset = 0;
                
                if (info.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                    bufferInfo->buffer = m_ubos[info.uboIndex].buffers[i];
                    bufferInfo->range = m_ubos[info.uboIndex].size;
                }
                else {
                    bufferInfo->buffer = m_dynamicUbos[info.uboIndex].ubo.buffers[i];
                    bufferInfo->range = m_dynamicUbos[info.uboIndex].sliceSize;
                }

                writer.pBufferInfo = bufferInfo.get();

                buffersInfo.push_back(std::move(bufferInfo));
            }

            if (info.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                // create in heap because it is passed by reference
                auto imageInfo = std::make_unique<VkDescriptorImageInfo>();
                imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo->imageView = info.texture->getImageView();
                imageInfo->sampler = info.texture->getSampler();

                writer.pImageInfo = imageInfo.get();

                imagesInfo.push_back(std::move(imageInfo));
            }

            descriptorWrites.push_back(writer);

        }
  
        vkUpdateDescriptorSets(logicalDevice, static_cast<u32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

        // clean for the next loop iteration
        descriptorWrites.clear();
        buffersInfo.clear();
        imagesInfo.clear();
    }
}

void rk::DescriptorSet::addSampler(const RawTexture* texture, u32 binding, ShaderStage stage) {
    LayoutInfo layout{};
    layout.texture = texture;
    layout.binding = binding;
    layout.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout.shaderStage = stage;

    m_layouts.push_back(layout);
}

i32 rk::DescriptorSet::addUbo(u64 size, u32 binding, ShaderStage stage) {
    i32 index = m_ubos.size();

    LayoutInfo layout{};
    layout.uboIndex = index;
    layout.binding = binding;
    layout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout.shaderStage = stage;

    m_layouts.push_back(layout);

    UboInfo uboInfo{};
    createUbo(uboInfo, size);

    m_ubos.push_back(uboInfo);

    return index;
}

i32 rk::DescriptorSet::addDynamicUbo(u32 sliceSize) {
    m_dynamicUboOffset = 0;
    m_dynamicUboIndex = m_dynamicUbos.size();

    m_dynamicUbos.push_back({ {}, {}, sliceSize });
    m_offsetsMask.push_back(0);

    return m_dynamicUboIndex;
}

u32 rk::DescriptorSet::addDynamicUboOffset() {
    u32 index = m_dynamicUbos[m_dynamicUboIndex].offsets.size();
    u32 offset = m_dynamicUboOffset;

    static const u32 alignedSize = static_cast<u32>(vulkanApp::getProperties().limits.minUniformBufferOffsetAlignment);

    // aligns the offset by alignedSize
    m_dynamicUboOffset += math::alignUp(m_dynamicUbos[m_dynamicUboIndex].sliceSize, alignedSize);

    m_dynamicUbos[m_dynamicUboIndex].offsets.push_back(offset);

    return index;
}

void rk::DescriptorSet::updateDynamicUbo(i32 uboIndex, i32 offsetIndex, u32 additionalOffset, u64 size, const void* data) {
    u32 offset = m_dynamicUbos[uboIndex].offsets[offsetIndex];

    assert(math::isAligned(offset, static_cast<u32>(vulkanApp::getProperties().limits.minUniformBufferOffsetAlignment)));

    updateUboBuffer(m_dynamicUbos[uboIndex].ubo, offset + additionalOffset, size, data);
}

void rk::DescriptorSet::createDynamicUbo(u32 binding, ShaderStage stage) {
    LayoutInfo layout{};
    layout.uboIndex = m_dynamicUboIndex;
    layout.binding = binding;
    layout.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout.shaderStage = stage;

    m_layouts.push_back(layout);

    // uses dynamicUboOffset as buffer size
    createUbo(m_dynamicUbos[m_dynamicUboIndex].ubo, m_dynamicUboOffset);
}

void rk::DescriptorSet::createUbo(UboInfo& uboInfo, u64 size) {
    auto logicalDevice = vulkanApp::getLogicalDevice();

    uboInfo.size = size;

    for (size_t i = 0; i < utl::FRAMES_COUNT; i++) {
        utl::createBuffer(size, uboInfo.buffers[i], uboInfo.memories[i], BufferUsage::UniformBuffer,
            MemoryType::HostVisible | MemoryType::HostCoherent);

        vkMapMemory(logicalDevice, uboInfo.memories[i], 0, size, 0, &uboInfo.mappedBuffers[i]);
    }
}

void rk::DescriptorSet::updateUboBuffer(UboInfo& uboInfo, u64 offset, u64 size, const void* data) {
    assert(offset + size <= uboInfo.size);

    std::memcpy(static_cast<char*>(uboInfo.mappedBuffers[vulkanApp::getImageIndex()]) + offset, data, size);
}

void rk::DescriptorSet::updateUboBufferAll(UboInfo& uboInfo, u64 offset, u64 size, const void* data) {
    assert(offset + size <= uboInfo.size);

    for (int i = 0; i < utl::FRAMES_COUNT; i++)
        std::memcpy(static_cast<char*>(uboInfo.mappedBuffers[i]) + offset, data, size);
}

void rk::DescriptorSet::destroyUbo(UboInfo& uboInfo) {
    auto logicalDevice = vulkanApp::getLogicalDevice();

    for (int i = 0; i < utl::FRAMES_COUNT; i++) {
        vkUnmapMemory(logicalDevice, uboInfo.memories[i]);
        vkDestroyBuffer(logicalDevice, uboInfo.buffers[i], nullptr);
        vkFreeMemory(logicalDevice, uboInfo.memories[i], nullptr);
    }
}