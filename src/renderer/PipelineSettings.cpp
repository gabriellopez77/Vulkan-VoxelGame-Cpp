#include "PipelineSettings.h"

#include "PushConstants.h"
#include "DescriptorSetLayout.h"


void rk::PipelineSettings::addDynamicState(VkDynamicState state) { dynamicStates.push_back(state); }

void rk::PipelineSettings::addBindings(u32 binding, VkVertexInputRate inputRate, u32 stride) {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = binding;
    bindingDescription.inputRate = inputRate;
    bindingDescription.stride = stride;

    this->bindings.push_back(bindingDescription);
}

void rk::PipelineSettings::addAttributes(u32 location, u32 binding, VkFormat format, u32 stride) {
    VkVertexInputAttributeDescription attribute;
    attribute.location = location;
    attribute.binding = binding;
    attribute.format = format;
    attribute.offset = stride;

    attributes.push_back(attribute);
}

void rk::PipelineSettings::addPushConstants(const PushConstants& constants) {
    pushConstants.push_back(constants.get());
}

void rk::PipelineSettings::addDescriptorSets(const DescriptorSetLayout& descriptorSets) {
    this->descriptorSets.push_back(descriptorSets.get());
}
