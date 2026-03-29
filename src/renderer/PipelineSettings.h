#pragma once

#include "Defs.h"

#include <vector>

#include <vulkan/vulkan.h>


namespace rk {
    // fwd
    class PushConstants;
    class DescriptorSetLayout;

    class PipelineSettings {
    public:
        const char* vertPath = nullptr;
        const char* fragPath = nullptr;

        void setShaders(const char* vertPath, const char* fragPath) {
            this->vertPath = vertPath;
            this->fragPath = fragPath;
        }

        void addDynamicState(VkDynamicState state);
        void addBindings(u32 binding, VkVertexInputRate inputRate, u32 stride);
        void addAttributes(u32 location, u32 binding, VkFormat format, u32 stride);
        void addPushConstants(const PushConstants& constants);
        void addDescriptorSets(const DescriptorSetLayout& descriptorSets);

        std::vector<VkDynamicState> dynamicStates;
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkPushConstantRange> pushConstants;
        std::vector<VkDescriptorSetLayout> descriptorSets;
    };
}