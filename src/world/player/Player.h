#pragma once

#include <glm/vec3.hpp>

#include "Camera.h"


// fwd
class Game;

class Player {
public:
    void start(Game* game);
    void update(float dt);

    Camera camera;

    glm::vec3 position{};
};