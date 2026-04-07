#pragma once


// fwd
namespace rk {
	class DescriptorSet;
}

namespace resources::descriptorSetManager {
	extern void start();

	extern rk::DescriptorSet* get(const char* name);
}