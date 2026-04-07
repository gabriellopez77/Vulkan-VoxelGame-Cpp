#include "Game.h"

#include <iostream>

#include "render/PipelineSettings.h"
#include "render/core/VulkanApp.h"
#include "render/DescriptorSet.h"

#include "resources/ArrayBuffer.h"
#include "resources/TextureManager.h"
#include "resources/DescriptorSetManager.h"
#include "math/Vec2.h"


struct VertexData {
    Vec2 vertices;
    Vec2 texCoords;
};

struct InstanceData {
    Vec3 position;
    Vec3 size;
};

constexpr VertexData vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f}},
    {{ 0.5f, -0.5f}, {0.0f, 0.0f}},
    {{ 0.5f,  0.5f}, {0.0f, 1.0f}},
    {{-0.5f,  0.5f}, {1.0f, 1.0f}}
};

constexpr u32 indices[] = {
    0, 1, 2, 2, 3, 0
};

i32 buffer1Index = 0;
i32 buffer2Index = 0;

constexpr int INSTANCE_COUNT = 2;

rk::DescriptorSet* globalDescriptor = nullptr;

void Game::start(Application* application) {
    m_application = application;

    resources::textueManager::start();
    resources::descriptorSetManager::start();

    attributesObject.addVertexBuffer(sizeof(VertexData), rk::VertexInputRate::VERTEX, &buffer1Index)
        .createVertices(sizeof(vertices), vertices, rk::UpdateType::ONE_TIME)
        .createIndices(sizeof(indices), indices, rk::UpdateType::ONE_TIME);
    attributesObject.setAttributes(0, rk::Formats::RG_F32, offsetof(VertexData, vertices));
    attributesObject.setAttributes(1, rk::Formats::RG_F32, offsetof(VertexData, texCoords));


    attributesObject.addVertexBuffer(sizeof(InstanceData), rk::VertexInputRate::INSTANCE, &buffer2Index)
       .createVertices(sizeof(InstanceData) * INSTANCE_COUNT, nullptr, rk::UpdateType::OFTEN);
    attributesObject.setAttributes(2, rk::Formats::RGB_F32, offsetof(InstanceData, position));
    attributesObject.setAttributes(3, rk::Formats::RGB_F32, offsetof(InstanceData, size));
    
    globalDescriptor = resources::descriptorSetManager::get("global");

    rk::PipelineSettings pipelineSettings;
    pipelineSettings.cullMode = rk::CullMode::DISABLE;
    pipelineSettings.enableBlending = true;
    pipelineSettings.enableDepthTest = true;

    pipelineSettings.setShaders(SHADERS_FOLDER"/vertex.vspv", SHADERS_FOLDER"/fragment.fspv");
    pipelineSettings.addDynamicState({ rk::DynamicState::VIEWPORT, rk::DynamicState::SCISSOR });
    pipelineSettings.AddAttributesObject(attributesObject);
    pipelineSettings.addDescriptorSet(globalDescriptor);


    pipeline.create(pipelineSettings);

    spritesRenderer.start();
}

void Game::update(f32 dt) {
    player.update(dt);
}

void Game::render() {
    auto command = rk::vulkanApp::getCurrentCommandBuffer();

    pipeline.bind(command);
    attributesObject.bind(command);

    const Matrix4 proj = player.camera.getProjectionMatrix();
    const Matrix4 view = player.camera.getViewMatrix();
    const auto projView = proj * view;

    globalDescriptor->updateUbo(0, 0, 64, &proj);
    globalDescriptor->updateUbo(0, 64, 64, &view);
    globalDescriptor->updateUbo(0, 128, 64, &projView);

    InstanceData instanceData[INSTANCE_COUNT] = {
        { {0, 0, -1}, {100, 100, 100} },
        { {0, 0, 1}, {10.f, 40.f, 10.f} },
    };

    attributesObject.update(buffer2Index, sizeof(instanceData), instanceData);

    vkCmdDrawIndexed(command, std::size(indices), INSTANCE_COUNT, 0, 0, 0);

    rk::SpriteVertices teste{};
    teste.size = { 100, 100 };
    teste.color = { 0, 255, 0, 255 };
    
    spritesRenderer.buffer.getEnd() = teste;

    spritesRenderer.draw();
}

void Game::resize(i32 width, i32 height) {
    player.camera.resize((f32)width, (f32)height);

    auto orthographic = Matrix4::orthographic(0.f, (f32)width, 0.f, (f32)height, -1.f, 1.f);
    resources::descriptorSetManager::get("global")->updateUboAll(0, 192, 64, &orthographic);
}