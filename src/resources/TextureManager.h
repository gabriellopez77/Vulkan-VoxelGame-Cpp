#pragma once

#include "Defs.h"


// fwd
namespace rk {
    class RawTexture;
}

namespace resources {
    class TextureAtlas;
}

namespace resources {
    extern void startTextures();
    extern const rk::RawTexture* getTexture(str name);
    extern const TextureAtlas* getTextureAtlas(str name);
}