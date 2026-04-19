#include "PipelineSettings.h"

#include "DescriptorSet.h"
#include "AttributesObject.h"


void rk::PipelineSettings::AddAttributesObject(const AttributesObject& attributesObject) {
    const auto& attributes = attributesObject.getAttributesInfo();
    const auto& bindings = attributesObject.getBindingDescription();

    this->attributes.reserve(attributes.size());
    this->bindings.reserve(bindings.size());

    for (const auto& attribute : attributes) {
        VkVertexInputAttributeDescription attributeDescription{};
        attributeDescription.location = attribute.location;
        attributeDescription.binding = attribute.binding;
        attributeDescription.format = static_cast<VkFormat>(attribute.format);
        attributeDescription.offset = attribute.offset;

        this->attributes.push_back(attributeDescription);
    }

    for (const auto& binding : bindings) {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = binding.binding;
        bindingDescription.inputRate = static_cast<VkVertexInputRate>(binding.inputRate);
        bindingDescription.stride = binding.stride;

        this->bindings.push_back(bindingDescription);
    }
}

void rk::PipelineSettings::addDescriptorSet(DescriptorSet* descriptorSet) {
    this->descriptorSets.push_back(descriptorSet);
    this->descriptorSetsLayout.push_back(descriptorSet->getLayout());
}
