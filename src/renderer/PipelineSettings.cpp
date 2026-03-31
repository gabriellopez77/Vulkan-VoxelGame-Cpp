#include "PipelineSettings.h"

#include "PushConstants.h"
#include "DescriptorSetLayout.h"
#include "VulkanEnums.h"


void rk::PipelineSettings::addDynamicState(DynamicStates state) { dynamicStates.push_back((VkDynamicState)state); }

void rk::PipelineSettings::addBindings(u32 binding, VertexInputRate inputRate, u32 stride) {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = binding;
    bindingDescription.inputRate = (VkVertexInputRate)inputRate;
    bindingDescription.stride = stride;

    this->bindings.push_back(bindingDescription);
    m_currentBinding = binding;
}

void rk::PipelineSettings::addAttributes(u32 location, VecFormats format, u32 stride) {
    VkVertexInputAttributeDescription attribute;
    attribute.location = location;
    attribute.binding = m_currentBinding;
    attribute.format = (VkFormat)format;
    attribute.offset = stride;

    attributes.push_back(attribute);
}

void rk::PipelineSettings::addPushConstants(const PushConstants& constants) {
    pushConstants.push_back(constants.get());
}

void rk::PipelineSettings::addDescriptorSets(const DescriptorSetLayout& descriptorSets) {
    this->descriptorSets.push_back(descriptorSets.get());
}
