#pragma once

#include <vulkan/vulkan.h>

#include "../defs.h"


namespace render {
    struct PipelineSettings {
        const char* vertPath = nullptr;
        const char* fragPath = nullptr;

        u32 dynamicStatesCount = 0;
        VkDynamicState* dynamicStates = nullptr;

        u32 bindingsCount = 0;
        VkVertexInputBindingDescription* bindings = nullptr;

        u32 attributesCount = 0;
        VkVertexInputAttributeDescription* attributes = nullptr;
    };
}