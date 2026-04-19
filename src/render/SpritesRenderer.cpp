#include "SpritesRenderer.h"

#include "core/PipelineSettings.h"
#include "core/VulkanEnums.h"
#include "core/VulkanApp.h"
#include "resources/DescriptorSetManager.h"


void rk::SpritesRenderer::start() {
    attributesObject.createVertexBuffer(sizeof(SPRITES_VERTICES), 0, SPRITES_VERTICES, sizeof(f32) * 4, VertexInputRate::Vertex, UpdateType::OneTime);
    attributesObject.createIndexBuffer(sizeof(SPRITES_INDICES), SPRITES_INDICES, UpdateType::OneTime);
    attributesObject.setAttributes(0, Formats::RgbaF32, 0);

    attributesObject.createVertexBuffer(sizeof(SpriteVertices) * MAX_SPRITES_COUNT, 0, nullptr, sizeof(SpriteVertices), VertexInputRate::Instance, UpdateType::Often);
    attributesObject.setAttributes(1, Formats::RgF32, offsetof(SpriteVertices, position));
    attributesObject.setAttributes(2, Formats::RgF32, offsetof(SpriteVertices, size));
    attributesObject.setAttributes(3, Formats::RgbaF32, offsetof(SpriteVertices, uv));
    attributesObject.setAttributes(4, Formats::RgbaU8, offsetof(SpriteVertices, color));
    attributesObject.setAttributes(5, Formats::RF32, offsetof(SpriteVertices, depth));

    PipelineSettings pipelineSettings;
    pipelineSettings.cullMode = CullMode::Disable;
    pipelineSettings.enableBlending = true;
    pipelineSettings.enableDepthTest = false;
    pipelineSettings.setShaders(SHADERS_FOLDER"/sprites.vspv", SHADERS_FOLDER"/sprites.fspv");
    pipelineSettings.addDynamicState({ DynamicState::Viewport, DynamicState::Scissor });
    pipelineSettings.AddAttributesObject(attributesObject);
    pipelineSettings.addDescriptorSet(resources::getDescriptorSet("global"));

    pipeline.create(pipelineSettings);
}

void rk::SpritesRenderer::draw() {
    auto command = rk::vulkanApp::getCurrentCommandBuffer();

    u32 instancesCount = buffer.length();

    if (instancesCount == 0)
        return;

    pipeline.bind(command);
    attributesObject.bind(command);
    attributesObject.updateVertexBuffer(1, buffer.sizeInBytes(), buffer.data());

    vkCmdDrawIndexed(command, std::size(rk::SPRITES_INDICES), instancesCount, 0, 0, 0);

    buffer.clear();
}