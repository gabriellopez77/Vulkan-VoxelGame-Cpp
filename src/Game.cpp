#include "Game.h"

#include <iostream>

#include "Inputs.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "renderer/PipelineSettings.h"
#include "renderer/VulkanApp.h"
#include "renderer/VulkanEnums.h"


struct VertexData {
    glm::vec2 vertices;
    glm::vec2 texCoords;
};

struct InstanceData {
    glm::vec3 position;
    glm::vec2 size;
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

constexpr int INSTANCE_COUNT = 5;

void Game::start(Application* application) {
    m_application = application;

    ubo.create(128);
    texture1.create(PROJECT_ROOT_PATH"/assets/textures/brick.png", rk::SamplerFilter::NEAREST, rk::SamplerMode::REPEAT);

    descriptorSet.addUbo(ubo, 0, rk::ShaderStage::VERTEX);
    descriptorSet.addSampler(texture1, 1, rk::ShaderStage::FRAGMENT);
    descriptorSet.create();

    rk::PipelineSettings pipelineSettings;
    pipelineSettings.setShaders(SHADERS_FOLDER"/vertex.spv", SHADERS_FOLDER"/fragment.spv");
    pipelineSettings.addDynamicState(rk::DynamicState::VIEWPORT);
    pipelineSettings.addDynamicState(rk::DynamicState::SCISSOR);

    pipelineSettings.addBindings(0, rk::VertexInputRate::VERTEX, sizeof(VertexData));
    pipelineSettings.addAttributes(0, rk::Formats::RG_F32, offsetof(VertexData, vertices));
    pipelineSettings.addAttributes(1, rk::Formats::RG_F32, offsetof(VertexData, texCoords));

    pipelineSettings.addBindings(1, rk::VertexInputRate::INSTANCE, sizeof(InstanceData));
    pipelineSettings.addAttributes(2, rk::Formats::RGB_F32, offsetof(InstanceData, position));
    pipelineSettings.addAttributes(3, rk::Formats::RG_F32, offsetof(InstanceData, size));

    pipelineSettings.addDescriptorSets(descriptorSet);

    pipeline.create(pipelineSettings);

    InstanceData instanceData[INSTANCE_COUNT] = {
        {{0, 296, 30}, {100, 100} },
        {{200, 0, 10}, {130, 56} },
        {{0, 99, -10}, {150, 20} },
        {{400, 423, 100}, {10, 90} },
        {{345, 45, 50}, {100, 80} },
    };

    vertexBuffer1.create(sizeof(vertices), vertices, sizeof(indices), indices);
    vertexBuffer2.create(sizeof(instanceData), instanceData);
}

void Game::update(float dt) {
    player.update(dt);
}

void Game::render() {
    auto command = rk::VulkanApp::get()->getCurrentCommandBuffer();

    pipeline.bind(command);
    vertexBuffer1.bind(command, 0);
    vertexBuffer2.bind(command, 1);
    descriptorSet.bind(command, pipeline.getLayout());

    glm::mat4 arr[2] = {
        player.camera.getProjectionMatrix(),
        player.camera.getViewMatrix()
    };
    ubo.updateSingle(0, 128, arr);

    vkCmdDrawIndexed(command, std::size(indices), INSTANCE_COUNT, 0, 0, 0);

    inputs::setMousePos(inputs::getMousePos());
}

void Game::resize(int width, int height) {
    player.camera.resize((float)width, (float)height);
}