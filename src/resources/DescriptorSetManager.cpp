#include "DescriptorSetManager.h"

#include <unordered_map>
#include <string>
#include <cassert>

#include "render/core/DescriptorSet.h"
#include "TextureAtlas.h"
#include "TextureManager.h"
#include "math/Matrix4.h"


namespace resources {
	std::unordered_map<std::string, rk::DescriptorSet*> descriptors;

	rk::DescriptorSet* add(const std::string& name);

	void startDescriptorSets() {
		auto atlas = resources::getTextureAtlas("sla");

		auto d1 = add("global");
		d1->addSampler(&atlas->rawTexture, 1, rk::ShaderStage::Fragment);
		d1->addUbo(sizeof(Matrix4) * 4, 0, rk::ShaderStage::Vertex);
		d1->create();
	}

	rk::DescriptorSet* getDescriptorSet(str name) {
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