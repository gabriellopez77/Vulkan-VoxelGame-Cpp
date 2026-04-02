#pragma once

#include "Defs.h"

#include <vector>

#include <vulkan/vulkan.h>


namespace rk {
    // fwd
    class PushConstants;
    class DescriptorSet;
    enum class Formats : i32;
    enum class ShaderStage : i32;
    enum class DynamicState : i32;
    enum class VertexInputRate : i32;

    class PipelineSettings {
    public:
        const char* vertPath = nullptr;
        const char* fragPath = nullptr;

        void setShaders(const char* vertPath, const char* fragPath) {
            this->vertPath = vertPath;
            this->fragPath = fragPath;
        }

        void addDynamicState(DynamicState state);
        void addBindings(u32 binding, VertexInputRate inputRate, u32 stride);
        void addAttributes(u32 location, Formats format, u32 stride);
        void addPushConstants(const PushConstants& constants);
        void addDescriptorSets(const DescriptorSet& descriptorSets);

        std::vector<VkDynamicState> dynamicStates;
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkPushConstantRange> pushConstants;
        std::vector<VkDescriptorSetLayout> descriptorSets;

    private:
        u32 m_currentBinding = 0;
    };
}