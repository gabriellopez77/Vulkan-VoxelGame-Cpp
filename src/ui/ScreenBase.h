#pragma once

#include "math/Vec2.h"

// fwd
namespace rk {
	class SpritesRenderer;
}

namespace ui {
	// fwd
	enum class ScreenId : int;

	class ScreenBase {
	public:
		ScreenBase(ScreenId id) : id(id) {}

		virtual void start() = 0;
		virtual void update(f32 dt) = 0;
		virtual void draw(rk::SpritesRenderer& renderer) = 0;
		virtual void resize() = 0;

		Vec2 screenSize;
		Vec2 screenCenter;
		bool started = false;
		bool canReturn = true;

		const ScreenId id;
	};
}