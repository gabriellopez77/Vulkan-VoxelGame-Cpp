#pragma once

#include "render/core/GraphicsPipeline.h"
#include "render/core/AttributesObject.h"
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

        void start();

        void draw();
    };
}
