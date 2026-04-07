#include "SpritesRenderer.h"

#include "PipelineSettings.h"
#include "VulkanEnums.h"
#include "render/core/VulkanApp.h"
#include "resources/DescriptorSetManager.h"


void rk::SpritesRenderer::start() {
    attributesObject.addVertexBuffer(sizeof(f32) * 4, VertexInputRate::VERTEX, nullptr)
        .createVertices(sizeof(SPRITES_VERTICES), SPRITES_VERTICES, UpdateType::ONE_TIME)
        .createIndices(sizeof(SPRITES_INDICES), SPRITES_INDICES, UpdateType::ONE_TIME);
    attributesObject.setAttributes(0, Formats::RGBA_F32, 0);

    attributesObject.addVertexBuffer(sizeof(SpriteVertices), VertexInputRate::INSTANCE, nullptr)
        .createVertices(sizeof(SpriteVertices) * MAX_SPRITES_COUNT, nullptr, UpdateType::OFTEN);
    attributesObject.setAttributes(1, Formats::RG_F32, offsetof(SpriteVertices, position));
    attributesObject.setAttributes(2, Formats::RG_F32, offsetof(SpriteVertices, size));
    attributesObject.setAttributes(3, Formats::RGBA_F32, offsetof(SpriteVertices, uv));
    attributesObject.setAttributes(4, Formats::RGBA_U8, offsetof(SpriteVertices, color));
    attributesObject.setAttributes(5, Formats::R_F32, offsetof(SpriteVertices, depth));

    PipelineSettings pipelineSettings;
    pipelineSettings.cullMode = CullMode::DISABLE;
    pipelineSettings.enableBlending = true;
    pipelineSettings.enableDepthTest = false;
    pipelineSettings.setShaders(SHADERS_FOLDER"/sprites.vspv", SHADERS_FOLDER"/sprites.fspv");
    pipelineSettings.addDynamicState({ DynamicState::VIEWPORT, DynamicState::SCISSOR });
    pipelineSettings.AddAttributesObject(attributesObject);
    pipelineSettings.addDescriptorSet(resources::descriptorSetManager::get("global"));

    pipeline.create(pipelineSettings);
}

void rk::SpritesRenderer::draw() {
    auto command = rk::vulkanApp::getCurrentCommandBuffer();

    u32 instancesCount = buffer.length();

    if (instancesCount == 0)
        return;

    pipeline.bind(command);
    attributesObject.bind(command);
    attributesObject.update(1, buffer.sizeInBytes(), buffer.data());

    vkCmdDrawIndexed(command, std::size(rk::SPRITES_INDICES), instancesCount, 0, 0, 0);

    buffer.clear();
}