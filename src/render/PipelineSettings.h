#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "Defs.h"


namespace rk {
    // fwd
    class DescriptorSet;
    enum class Formats : i32;
    enum class ShaderStage : i32;
    enum class DynamicState : i32;
    enum class VertexInputRate : i32;
    enum class CullMode : u32;

    class PipelineSettings {
    public:
        ~PipelineSettings() {
            delete pushConstantRange;
        }

        const char* vertPath = nullptr;
        const char* fragPath = nullptr;

        void setShaders(const char* vertPath, const char* fragPath) {
            this->vertPath = vertPath;
            this->fragPath = fragPath;
        }

        void addDynamicState(DynamicState state);
        void addBindings(u32 binding, VertexInputRate inputRate, u32 stride);
        void addAttributes(u32 location, Formats format, u32 stride);
        void addPushConstant(u32 offset, u32 size, ShaderStage stage);
        void addDescriptorSet(const DescriptorSet& descriptorSets);

        CullMode cullMode = (CullMode)VK_CULL_MODE_BACK_BIT;
        bool enableBlending = false;
        bool enableDepthTest = true;

        VkPushConstantRange* pushConstantRange = nullptr;

        std::vector<VkDynamicState> dynamicStates;
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkDescriptorSetLayout> descriptorSets;

    private:
        u32 m_currentBinding = 0;
    };
}