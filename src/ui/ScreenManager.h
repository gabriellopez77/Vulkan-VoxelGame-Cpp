#pragma once

#include "Defs.h"


namespace ui {
	enum class ScreenId : int {
		StartScreen,
	};
}

namespace ui::screenManager {
	void start();
	bool currentScreenIs(ScreenId id);
	void change(ScreenId id);
	void returnBack();

	void updateUi(f32 dt);
	void drawUi();
	void resize(i32 width, i32 height);
}