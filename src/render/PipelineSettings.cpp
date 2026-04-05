#include "PipelineSettings.h"

#include "DescriptorSet.h"
#include "AttributesObject.h"


void rk::PipelineSettings::AddAttributesObject(const AttributesObject& attributesObject) {
    const auto& attributes = attributesObject.getAttributesInfo();
    const auto& bindings = attributesObject.getBuffersInfo();

    this->attributes.reserve(attributes.size());
    this->bindings.reserve(bindings.size());

    for (const auto& attribute : attributes) {
        VkVertexInputAttributeDescription attributeDescription{};
        attributeDescription.location = attribute.location;
        attributeDescription.binding = attribute.binding;
        attributeDescription.format = (VkFormat)attribute.format;
        attributeDescription.offset = attribute.offset;

        this->attributes.push_back(attributeDescription);
    }

    for (const auto& binding : bindings) {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = binding.binding;
        bindingDescription.inputRate = (VkVertexInputRate)binding.inputRate;
        bindingDescription.stride = binding.stride;

        this->bindings.push_back(bindingDescription);
    }
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
