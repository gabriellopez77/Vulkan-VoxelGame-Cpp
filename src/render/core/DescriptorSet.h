#pragma once

#include <vector>

#include "Utils.h"


namespace rk {
    // fwd
    class RawTexture;
    enum class ShaderStage : i32;

    class DescriptorSet {
        struct UboInfo {
            u64 size = 0;

            VkBuffer buffers[utl::FRAMES_COUNT] = {};
            VkDeviceMemory memories[utl::FRAMES_COUNT] = {};
            void* mappedBuffers[utl::FRAMES_COUNT] = {};
        };
        struct LayoutInfo {
            const RawTexture* texture;
            i32 uboIndex;
            u32 binding;
            i32 type;
            ShaderStage shaderStage;
        };

        struct DynamicUboInfo {
            std::vector<u32> offsets;
            UboInfo ubo;
            u32 sliceSize;
        };

    public:
        void create();

        void addSampler(const RawTexture* texture, u32 binding, ShaderStage stage);
        i32 addUbo(u64 size, u32 binding, ShaderStage stage);

        void updateUbo(i32 uboIndex, u64 offset, u64 size, const void* data) {
            updateUboBuffer(m_ubos[uboIndex], offset, size, data);
        }

        void updateUboAll(i32 uboIndex, u64 offset, u64 size, const void* data) {
            updateUboBufferAll(m_ubos[uboIndex], offset, size, data);
        }

        i32 addDynamicUbo(u32 sliceSize);
        u32 addDynamicUboOffset();
        void createDynamicUbo(u32 binding, ShaderStage stage);
        void updateDynamicUbo(i32 dynamicUboIndex, i32 dynamicUboOffsetIndex, u32 additionalOffset, u64 size, const void* data);

        void enableDynamicUboOffset(i32 dynamicUboIndex, i32 offsetIndex) {
            m_offsetsMask[dynamicUboIndex] = m_dynamicUbos[dynamicUboIndex].offsets[offsetIndex];
        }

        void disableDynamicUboOffsets() {
            for (i32 i = 0; i < m_offsetsMask.size(); i++)
                m_offsetsMask[i] = 0;
        }

        const std::vector<u32>& getDynamicUboOffsets() const { return m_offsetsMask; }

        VkDescriptorSetLayout getLayout() const { return m_descriptorSetLayout; }
        VkDescriptorSet getVkDescriptor(i32 i) const { return m_descriptorSets[i]; }

    private:
        void createUbo(UboInfo& uboInfo, u64 size);
        void updateUboBuffer(UboInfo& uboInfo, u64 offset, u64 size, const void* data);
        void updateUboBufferAll(UboInfo& uboInfo, u64 offset, u64 size, const void* data);
        void destroyUbo(UboInfo& uboInfo);

        std::vector<UboInfo> m_ubos;
        std::vector<DynamicUboInfo> m_dynamicUbos;
        std::vector<LayoutInfo> m_layouts;
      
        // used for enable dynamic offsets used by draw call
        std::vector<u32> m_offsetsMask;

        // current dynamic ubo offset
        u32 m_dynamicUboOffset = 0;
        // current dynamic ubo index
        i32 m_dynamicUboIndex = 0;

        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;
        VkDescriptorSet m_descriptorSets[utl::FRAMES_COUNT] = {};
    };
}