#pragma once

#include "Defs.h"


// fwd
namespace rk {
	class DescriptorSet;
}

namespace resources {
	extern void startDescriptorSets();

	extern rk::DescriptorSet* getDescriptorSet(str name);
}