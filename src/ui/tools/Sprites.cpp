#include "Sprites.h"

#include "render/SpritesRenderer.h"


void ui::tools::Sprite::setTexture(str name) {

}

void ui::tools::Sprite::draw(rk::SpritesRenderer& renderer) {
	renderer.buffer.getEnd() = {
        getPos(),
        getSize(),
        m_uv,
        color,
        0
	};
}