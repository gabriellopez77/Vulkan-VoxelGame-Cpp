#pragma once

#include "render/DescriptorSet.h"
//#include "render/GraphicsPipeline.h"
//#include "render/AttributesObject.h"

#include "render/SpritesRenderer.h"

#include "world/player/Player.h"


// fwd
class Application;

class Game {
public:
    void start(Application* application);
    void update(f32 dt);
    void render();
    void resize(i32 width, i32 height);

    Player player;

private:
    rk::SpritesRenderer spritesRenderer;
    rk::AttributesObject attributesObject;
    rk::GraphicsPipeline pipeline;
    rk::DescriptorSet descriptorSet;

    Application* m_application = nullptr;

};