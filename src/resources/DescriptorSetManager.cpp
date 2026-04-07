#include "DescriptorSetManager.h"

#include <unordered_map>
#include <string>
#include <cassert>

#include "render/DescriptorSet.h"
#include "TextureManager.h"
#include "math/Matrix4.h"


namespace resources::descriptorSetManager {
	std::unordered_map<std::string, rk::DescriptorSet*> descriptors;

	rk::DescriptorSet* add(const std::string& name);

	void start() {
		auto tex = resources::textueManager::get("sla");

		auto d1 = add("global");
		d1->addSampler(tex, 1, rk::ShaderStage::FRAGMENT);
		d1->addUbo(sizeof(Matrix4) * 4, 0, rk::ShaderStage::VERTEX);
		d1->create();
	}

	rk::DescriptorSet* get(const char* name) {
		auto it = descriptors.find(name);

		assert(it != descriptors.end());

		return it->second;
	}

	rk::DescriptorSet* add(const std::string& name) {
		auto descriptor = new rk::DescriptorSet();

		descriptors.emplace(name, descriptor);

		return descriptor;
	}
}