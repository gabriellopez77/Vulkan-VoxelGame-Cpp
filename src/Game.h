#pragma once

#include "render/DescriptorSet.h"
#include "render/GraphicsPipeline.h"
#include "render/Ubo.h"
#include "render/VertexBuffer.h"
#include "render/Texture.h"
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