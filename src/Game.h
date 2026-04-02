#pragma once

#include "renderer/DescriptorSet.h"
#include "renderer/GraphicsPipeline.h"
#include "renderer/Ubo.h"
#include "renderer/VertexBuffer.h"
#include "renderer/Texture.h"
#include "world/player/Player.h"


// fwd
class Application;

class Game {
public:
    void start(Application* application);
    void update(float dt);
    void render();
    void resize(int width, int height);

    Player player;

private:
    rk::Ubo ubo;
    rk::VertexBuffer vertexBuffer1;
    rk::VertexBuffer vertexBuffer2;
    rk::GraphicsPipeline pipeline;
    rk::DescriptorSet descriptorSet;
    rk::Texture texture1;

    Application* m_application = nullptr;
};