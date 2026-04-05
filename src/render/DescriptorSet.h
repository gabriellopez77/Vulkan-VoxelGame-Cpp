#pragma once

#include <vector>

#include "render/Ubo.h"


namespace rk {
    // fwd
    class Texture;
    enum class ShaderStage : i32;

    class DescriptorSet {
        struct LayoutInfo {
            const Texture* texture;
            i32 uboIndex;
            u32 binding;
            i32 type;
            ShaderStage shaderStage;
        };

        struct DynamicUboInfo {
            std::vector<u32> offsets;
            Ubo buffer;
            u32 sliceSize;
        };

    public:
        void create();
        void bind(VkCommandBuffer command, VkPipelineLayout pipelineLayout);

        void addSampler(const Texture* texture, u32 binding, ShaderStage stage);
        i32 addUbo(u64 size, u32 binding, ShaderStage stage);

        void updateUbo(i32 uboIndex, u64 offset, u64 size, const void* data) {
            m_ubos[uboIndex].update(offset, size, data);
        }


        i32 addDynamicUbo(u32 sliceSize);
        u32 addDynamicUboOffset();
        void createDynamicUbo(u32 binding, ShaderStage stage);
        void updateDynamicUbo(i32 dynamicUboIndex, i32 dynamicUboOffsetIndex, u32 additionalOffset, u64 size, const void* data);

        void enableDynamicUboOffset(i32 dynamicUboIndex, i32 offsetIndex) {
            m_offsetsMask[dynamicUboIndex] = m_dynamicUbos[dynamicUboIndex].offsets[offsetIndex];
        }

        VkDescriptorSetLayout get() const { return m_descriptorSetLayout; }

    private:
        std::vector<Ubo> m_ubos;
        std::vector<DynamicUboInfo> m_dynamicUbos;
        std::vector<LayoutInfo> m_layouts;

        // dynamic buffers offsets
        //std::vector<> m_dynamicUbosInfo;
      
        // used for enable offsets used by draw call
        std::vector<u32> m_offsetsMask;

        // current dynamic ubo offset
        u32 m_dynamicUboOffset = 0;
        // current dynamic ubo index
        i32 m_dynamicUboIndex = 0;

        VkDescriptorSetLayout m_descriptorSetLayout = nullptr;
        VkDescriptorSet m_descriptorSets[utl::FRAMES_COUNT] = {};
    };
}