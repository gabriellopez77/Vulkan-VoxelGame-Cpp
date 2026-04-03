#pragma once

#include "math/Vec3.h"

#include "Camera.h"


// fwd
class Game;

class Player {
public:
    void start(Game* game);
    void update(float dt);

    Camera camera;

    Vec3 position;
};