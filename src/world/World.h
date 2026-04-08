#pragma once

#include "Defs.h"


namespace world {
	struct UpdateArgs {
		f32 dt;
		f32 time;
		i32 state;
	};

	extern void start();
	extern void update(const UpdateArgs& args);
}