#include "Application.h"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/GraphicsPipeline.h"
#include "renderer/PushConstants.h"
#include "renderer/DescriptorSetLayout.h"
#include "renderer/PipelineSettings.h"
#include "renderer/Ubo.h"
#include "renderer/VertexBuffer.h"
#include "renderer/VulkanApp.h"
#include "renderer/VulkanEnums.h"


struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

constexpr Vertex vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

constexpr u32 indices[] = {
    0, 1, 2, 2, 3, 0
};

void render();
void start();

rk::GraphicsPipeline pipeline;
rk::PushConstants pushConstants;
rk::VertexBuffer vertexBuffer;
//rk::Ubo ubo;
//rk::DescriptorSetLayout descriptorSetLayout;

Application app;

i32 main() {
    app.initWindow(1000, 800, "Vulkan App");
    app.setRenderFunc(render);
    app.setStartFunc(start);
    app.initVulkan();

    app.run();

    app.clearVulkan();
}

glm::mat4 model1(1.f);
glm::mat4 model2(1.f);

void start() {
    //ubo.create(128);
    //descriptorSetLayout.create(0, rk::ShaderStage::VERTEX);
    //descriptorSetLayout.setUbo(ubo, 0);
    pushConstants.create(128, rk::ShaderStage::VERTEX);

    rk::PipelineSettings pipelineSettings;
    pipelineSettings.setShaders(SHADERS_FOLDER"/vertex.spv", SHADERS_FOLDER"/fragment.spv");
    pipelineSettings.addDynamicState(rk::DynamicStates::VIEWPORT);
    pipelineSettings.addDynamicState(rk::DynamicStates::SCISSOR);

    pipelineSettings.addBindings(0, rk::VertexInputRate::VERTEX, sizeof(Vertex));
    pipelineSettings.addAttributes(0, rk::VecFormats::RG32_F32, offsetof(Vertex, pos));
    pipelineSettings.addAttributes(1, rk::VecFormats::RGB32_F32, offsetof(Vertex, color));

    pipelineSettings.addPushConstants(pushConstants);
    //pipelineSettings.addDescriptorSets(descriptorSetLayout);

    pipeline.create(pipelineSettings);

    vertexBuffer.create(sizeof(vertices), vertices, sizeof(indices), indices);
}

void render() {
    auto command = rk::VulkanApp::get()->getCurrentCommandBuffer();

    pipeline.bind(command);
    vertexBuffer.bind(command);
    //descriptorSetLayout.bind(command, pipeline.getLayout());

    float time = Application::Time;

    model1 = glm::translate(glm::mat4(1.f), glm::vec3(app.getWindowWidth() / 2.f, app.getWindowHeight() / 2.f, 0.f));
    model1 = glm::rotate(model1,  glm::radians(time) * 20.f, glm::vec3(0.f, 0.f, 1.f));
    model1 = glm::scale(model1, glm::vec3(100.f));

    glm::mat4 matrices[] = {
        glm::ortho(0.f, (float)app.getWindowWidth(), 0.f, (float)app.getWindowHeight(), -10.f, 10.f),
        model1,
    };

    //ubo.update(0, sizeof(matrices), matrices);

    pushConstants.bind(command, pipeline.getLayout(), sizeof(matrices), matrices);
    vkCmdDrawIndexed(command, std::size(indices), 1, 0, 0, 0);
}