#include "Game.h"

#include <iostream>

#include "Application.h"
#include "Inputs.h"

#include "render/PipelineSettings.h"
#include "render/core/VulkanApp.h"


#include "resources/ArrayBuffer.h"
#include "resources/TextureManager.h"

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

i32 uboIndex;
i32 buffer1Index = 0;
i32 buffer2Index = 0;

constexpr int INSTANCE_COUNT = 2;

void Game::start(Application* application) {
    m_application = application;

    resources::textueManager::start();

    auto tex = resources::textueManager::get("sla");

    descriptorSet.addSampler(tex, 1, rk::ShaderStage::FRAGMENT);
    uboIndex = descriptorSet.addUbo(sizeof(Matrix4) * 2, 0, rk::ShaderStage::VERTEX);  
    descriptorSet.create();




    attributesObject.addVertexBuffer(sizeof(VertexData), rk::VertexInputRate::VERTEX, &buffer1Index)
        .createVertices(sizeof(vertices), vertices, rk::VertexBufferType::ONE_TIME)
        .createIndices(sizeof(indices), indices, rk::VertexBufferType::ONE_TIME);
    attributesObject.setAttributes(0, rk::Formats::RG_F32, offsetof(VertexData, vertices));
    attributesObject.setAttributes(1, rk::Formats::RG_F32, offsetof(VertexData, texCoords));


    attributesObject.addVertexBuffer(sizeof(InstanceData), rk::VertexInputRate::INSTANCE, &buffer2Index)
       .createVertices(sizeof(InstanceData) * INSTANCE_COUNT, nullptr, rk::VertexBufferType::RAM);
    attributesObject.setAttributes(2, rk::Formats::RGB_F32, offsetof(InstanceData, position));
    attributesObject.setAttributes(3, rk::Formats::RGB_F32, offsetof(InstanceData, size));

    rk::PipelineSettings pipelineSettings;
    pipelineSettings.cullMode = rk::CullMode::DISABLE;
    pipelineSettings.enableBlending = true;
    pipelineSettings.enableDepthTest = true;

    pipelineSettings.setShaders(SHADERS_FOLDER"/vertex.spv", SHADERS_FOLDER"/fragment.spv");
    pipelineSettings.addDynamicState({ rk::DynamicState::VIEWPORT, rk::DynamicState::SCISSOR });

    pipelineSettings.AddAttributesObject(attributesObject);
    pipelineSettings.addDescriptorSet(descriptorSet);

    pipeline.create(pipelineSettings);
}

void Game::update(float dt) {
    player.update(dt);
}

void Game::render() {
    auto command = rk::vulkanApp::getCurrentCommandBuffer();

    pipeline.bind(command);
    descriptorSet.bind(command, pipeline.getLayout());
    attributesObject.bind(command);

    const auto& proj = player.camera.getProjectionMatrix();
    const auto& view = player.camera.getViewMatrix();

    descriptorSet.updateUbo(uboIndex, 0, 64, &proj);
    descriptorSet.updateUbo(uboIndex, 64, 64, &view);

    InstanceData instanceData[INSTANCE_COUNT] = {
        { {0, 0, -1}, {100, 100, 100} },
        { {0, 0, 1}, {10.f, 40.f, 10.f} },
    };

    attributesObject.update(buffer2Index, sizeof(instanceData), instanceData);

    vkCmdDrawIndexed(command, std::size(indices), INSTANCE_COUNT, 0, 0, 0);
}

void Game::resize(int width, int height) {
    player.camera.resize((float)width, (float)height);
}