#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "VulkanEnums.h"


namespace rk {
    // fwd
    class DescriptorSet;
    class AttributesObject;

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

        void addDynamicState(const std::vector<DynamicState>& dynamicStates) {
            this->dynamicStates.reserve(dynamicStates.size());

            for (int i = 0; i < dynamicStates.size(); i++)
                this->dynamicStates.push_back((VkDynamicState)dynamicStates[i]);
        }
        void AddAttributesObject(const AttributesObject& attributesObject);
        void addPushConstant(u32 offset, u32 size, ShaderStage stage);
        void addDescriptorSet(const DescriptorSet& descriptorSets);

        CullMode cullMode = CullMode::BACK;
        bool enableBlending = false;
        bool enableDepthTest = true;

        VkPushConstantRange* pushConstantRange = nullptr;

        std::vector<VkDynamicState> dynamicStates;
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkDescriptorSetLayout> descriptorSets;
    };
}