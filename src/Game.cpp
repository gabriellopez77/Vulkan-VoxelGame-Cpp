#include "Game.h"

#include <iostream>

#include "render/SpritesRenderer.h"
#include "render/core/PipelineSettings.h"
#include "render/core/VulkanApp.h"
#include "render/core/DescriptorSet.h"

#include "world/player/Player.h"

#include "resources/TextureManager.h"
#include "resources/DescriptorSetManager.h"
#include "math/Vec2.h"
#include "ui/ScreenManager.h"


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

namespace game {
    Player player;

    bool inWorld;

    rk::AttributesObject attributesObject;
    rk::GraphicsPipeline pipeline;

    void start() {
        resources::startTextures();
        resources::startDescriptorSets();

        std::cout << "AAAAAAAAA\n";
        ui::screenManager::start();

        buffer1Index = attributesObject.createVertexBuffer(sizeof(vertices), 0, vertices, sizeof(VertexData), rk::VertexInputRate::Vertex, rk::UpdateType::OneTime);
        attributesObject.createIndexBuffer(sizeof(indices), indices, rk::UpdateType::OneTime);
        attributesObject.setAttributes(0, rk::Formats::RgF32, offsetof(VertexData, vertices));
        attributesObject.setAttributes(1, rk::Formats::RgF32, offsetof(VertexData, texCoords));


        buffer2Index = attributesObject.createVertexBuffer(sizeof(InstanceData) * INSTANCE_COUNT, 0, nullptr, sizeof(InstanceData), rk::VertexInputRate::Instance, rk::UpdateType::Often);
        attributesObject.setAttributes(2, rk::Formats::RgbF32, offsetof(InstanceData, position));
        attributesObject.setAttributes(3, rk::Formats::RgbF32, offsetof(InstanceData, size));
    
        globalDescriptor = resources::getDescriptorSet("global");

        rk::PipelineSettings pipelineSettings;
        pipelineSettings.cullMode = rk::CullMode::Disable;
        pipelineSettings.enableBlending = true;
        pipelineSettings.enableDepthTest = true;

        pipelineSettings.setShaders(SHADERS_FOLDER"/vertex.vspv", SHADERS_FOLDER"/fragment.fspv");
        pipelineSettings.addDynamicState({ rk::DynamicState::Viewport, rk::DynamicState::Scissor });
        pipelineSettings.AddAttributesObject(attributesObject);
        pipelineSettings.addDescriptorSet(globalDescriptor);


        pipeline.create(pipelineSettings);
    }

    void update(f32 dt) {
        player.update(dt);

        ui::screenManager::updateUi(dt);     
    }

    void render() {
        auto command = rk::vulkanApp::getCurrentCommandBuffer();

        pipeline.bind(command);
        attributesObject.bind(command);

        const Matrix4& proj = player.camera.getProjectionMatrix();
        const Matrix4& view = player.camera.getViewMatrix();
        const auto projView = proj * view;

        globalDescriptor->updateUbo(0, 0, 64, &proj);
        globalDescriptor->updateUbo(0, 64, 64, &view);
        globalDescriptor->updateUbo(0, 128, 64, &projView);

        InstanceData instanceData[INSTANCE_COUNT] = {
            { {0, 0, -1}, {100, 100, 100} },
            { {0, 0, 1}, {10.f, 40.f, 10.f} },
        };

        attributesObject.updateVertexBuffer(buffer2Index, sizeof(instanceData), instanceData);

        vkCmdDrawIndexed(command, std::size(indices), INSTANCE_COUNT, 0, 0, 0);

        ui::screenManager::drawUi();
    }

    void resize(i32 width, i32 height) {
        player.camera.resize(static_cast<f32>(width), static_cast<f32>(height));

        auto orthographic = Matrix4::orthographic(0.f, static_cast<f32>(width), 0.f, static_cast<f32>(height), -1.f, 1.f);
        resources::getDescriptorSet("global")->updateUboAll(0, 192, 64, &orthographic);

        ui::screenManager::resize(width, height);
    }
}