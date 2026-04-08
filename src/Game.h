#pragma once

#include "Defs.h"


namespace game {
    extern void start();
    extern void update(f32 dt);
    extern void render();
    extern void resize(i32 width, i32 height);
}