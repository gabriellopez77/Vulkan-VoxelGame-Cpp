#include "Game.h"

#include <iostream>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Application.h"
#include "Inputs.h"
#include "glm/fwd.hpp"
#include "render/PipelineSettings.h"
#include "render/core/VulkanApp.h"
#include "render/VulkanEnums.h"


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
    pipelineSettings.cullMode = rk::CullMode::DISABLE;
    pipelineSettings.enableBlending = true;
    pipelineSettings.enableDepthTest = true;

    pipelineSettings.setShaders(SHADERS_FOLDER"/vertex.spv", SHADERS_FOLDER"/fragment.spv");
    pipelineSettings.addDynamicState(rk::DynamicState::VIEWPORT);
    pipelineSettings.addDynamicState(rk::DynamicState::SCISSOR);

    pipelineSettings.addBindings(0, rk::VertexInputRate::VERTEX, sizeof(VertexData));
    pipelineSettings.addAttributes(0, rk::Formats::RG_F32, offsetof(VertexData, vertices));
    pipelineSettings.addAttributes(1, rk::Formats::RG_F32, offsetof(VertexData, texCoords));

    pipelineSettings.addBindings(1, rk::VertexInputRate::INSTANCE, sizeof(InstanceData));
    pipelineSettings.addAttributes(2, rk::Formats::RGB_F32, offsetof(InstanceData, position));
    pipelineSettings.addAttributes(3, rk::Formats::RG_F32, offsetof(InstanceData, size));

    pipelineSettings.addDescriptorSet(descriptorSet);
    pipelineSettings.addPushConstant(0, 64, rk::ShaderStage::VERTEX);

    pipeline.create(pipelineSettings);

    InstanceData instanceData[INSTANCE_COUNT] = {
        {{0,  29,  30}, {100, 100} },
        {{20, 0,   10}, {130, 56} },
        {{0,  99, -10}, {150, 20} },
        {{40, 42,  10}, {10,  90} },
        {{34, 45,  50}, {100, 80} },
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

    auto proj = player.camera.getProjectionMatrix();
    auto view =player.camera.getViewMatrix();

    ubo.updateSingle(0, 64, &proj);
    ubo.updateSingle(64, 64, &view);

    Matrix4 model(1.f);
    model.rotate(Application::Time * 25.f, {1.f, 0.f, 0.f});
    model.translate({0, 0, -1});
    model.scale({100, 100, 100});

    glm::mat4 model2(1.f);
    model2 = glm::rotate(model2, glm::radians(Application::Time * 25.f), glm::vec3(1.f, 0.f, 0.f));
    model2 = glm::translate(model2, glm::vec3(0.f, 0.f, 1));
    model2 = glm::scale(model2, glm::vec3(100.f, 100.f, 100.f));

    pipeline.bindPushConstant(command, 64, &model);
    vkCmdDrawIndexed(command, std::size(indices), INSTANCE_COUNT, 0, 0, 0);

    model = Matrix4(1.f);

    model.translate(Vec3(0.f, 0.f, 1));
    model.scale(Vec3(10.f, 10.f, 10.f));
    pipeline.bindPushConstant(command, 64, &model);
    vkCmdDrawIndexed(command, std::size(indices), INSTANCE_COUNT, 0, 0, 0);

    inputs::setMousePos(inputs::getMousePos());
}

void Game::resize(int width, int height) {
    player.camera.resize((float)width, (float)height);
}