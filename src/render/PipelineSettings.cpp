#include "PipelineSettings.h"

#include "DescriptorSet.h"
#include "VulkanEnums.h"


void rk::PipelineSettings::addDynamicState(DynamicState state) { dynamicStates.push_back((VkDynamicState)state); }

void rk::PipelineSettings::addBindings(u32 binding, VertexInputRate inputRate, u32 stride) {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = binding;
    bindingDescription.inputRate = (VkVertexInputRate)inputRate;
    bindingDescription.stride = stride;

    this->bindings.push_back(bindingDescription);
    m_currentBinding = binding;
}

void rk::PipelineSettings::addAttributes(u32 location, Formats format, u32 stride) {
    VkVertexInputAttributeDescription attribute;
    attribute.location = location;
    attribute.binding = m_currentBinding;
    attribute.format = (VkFormat)format;
    attribute.offset = stride;

    attributes.push_back(attribute);
}

void rk::PipelineSettings::addPushConstant(u32 offset, u32 size, ShaderStage stage) {
    delete pushConstantRange;

    pushConstantRange = new VkPushConstantRange();
    pushConstantRange->offset = offset;
    pushConstantRange->size = size;
    pushConstantRange->stageFlags = (VkShaderStageFlags)stage;
}

void rk::PipelineSettings::addDescriptorSet(const DescriptorSet& descriptorSets) {
    this->descriptorSets.push_back(descriptorSets.get());
}
