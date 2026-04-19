#include "TextureManager.h"

#include <unordered_map>
#include <cassert>

#include "render/core/RawTexture.h"

#include "resources/TextureAtlas.h"


namespace resources {
    std::unordered_map<std::string, rk::RawTexture*> rawTextures;
    std::unordered_map<std::string, TextureAtlas*> texturesAtlas;

    void addTexture(str name, str path, rk::SamplerFilter filter, rk::SamplerMode mode);
    void addAtlas(str name, str path, rk::SamplerFilter filter, rk::SamplerMode mode);

    void startTextures() {
        addAtlas("sla", PROJECT_ROOT_PATH"/assets/textures", rk::SamplerFilter::Nearest, rk::SamplerMode::Repeat);
    }

    const rk::RawTexture* getTexture(str name) {
        auto it = rawTextures.find(name);

        assert(it != rawTextures.end());

        return it->second;
    }

    const TextureAtlas* getTextureAtlas(str name) {
        auto it = texturesAtlas.find(name);

        assert(it != texturesAtlas.end());

        return it->second;
    }

    void addTexture(str name, str path, rk::SamplerFilter filter, rk::SamplerMode mode) {
        auto texture = new rk::RawTexture();
        texture->create(path, filter, mode);

        rawTextures.emplace(name, texture);
    }

    void addAtlas(str name, str path, rk::SamplerFilter filter, rk::SamplerMode mode) {
        auto atlas = new TextureAtlas(path);
        atlas->rawTexture.create(atlas->data(), atlas->width(), atlas->height(), filter, mode);

        texturesAtlas.emplace(name, atlas);
    }
}