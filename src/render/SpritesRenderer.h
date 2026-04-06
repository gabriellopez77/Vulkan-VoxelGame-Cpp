#pragma once

#include "render/GraphicsPipeline.h"
#include "render/AttributesObject.h"
#include "resources/ArrayBuffer.h"
#include "VerticesData.h"


namespace rk {
    // fwd
    class DescriptorSet;

    class SpritesRenderer {
    public:
        resources::ArrayBuffer<SpriteVertices> buffer{MAX_SPRITES_COUNT};

        rk::AttributesObject attributesObject;
        rk::GraphicsPipeline pipeline;

        void start(const DescriptorSet& descriptorSet);

        void draw();
    };
}
