#pragma once

#include "UiElement.h"
#include "math/Color4b.h"
#include "math/Vec4.h"


namespace rk {
	class SpritesRenderer;
}

namespace ui::tools {
	class Sprite : public UiElement {
	public:
		void setPos(Vec2 pos) override { m_pos = pos; }
		void setSize(Vec2 size) override { m_size = size; }

		void setTexture(const Vec4& uv) { m_uv = uv; }
		void setTexture(str name);

		void draw(rk::SpritesRenderer& renderer);

		Color4b color = { 0, 0, 0, 255 };

	private:
		Vec4 m_uv;
	};
}