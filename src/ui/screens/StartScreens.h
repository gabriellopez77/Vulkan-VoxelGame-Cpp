#pragma once 

#include "ui/ScreenBase.h"


namespace ui{
	class StartScreen : public ScreenBase {
	public:
		StartScreen(ScreenId id) : ScreenBase(id) {}

		void start() override;
		void update(f32 dt) override;
		void draw(rk::SpritesRenderer& renderer) override;
		void resize() override;
	};
}