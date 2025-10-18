//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <cstring>

#include "bgfx/bgfx.h"
#include "nlohmann/json.hpp"

#include "OtLog.h"
#include "OtText.h"

#include "OtGlm.h"
#include "OtInstances.h"


//
//	OtInstances::OtInstances
//

OtInstances::OtInstances() {
	instances = std::make_shared<std::vector<glm::mat4>>();
}


//
//	OtInstances::clear
//

void OtInstances::clear() {
	instances = std::make_shared<std::vector<glm::mat4>>();
	incrementVersion();
}


//
//	OtInstances::load
//

void OtInstances::load(const std::string& path) {
	// load instances from file
	std::string text;
	OtText::load(path, text);
	auto data = nlohmann::json::parse(text);

	// clear list and add instances
	instances = std::make_shared<std::vector<glm::mat4>>();

	for (auto element : data.items()) {
		glm::mat4 instance;
		glm::from_json(element.value(), instance);
		instances->emplace_back(instance);
	}

	// increment version number
	incrementVersion();
}


//
//	OtInstances::save
//

void OtInstances::save(const std::string& path) {
	// create json
	auto data = nlohmann::json::array();

	for (auto& instance : *instances) {
		data.emplace_back(instance);
	}

	// write instances to file
	OtText::save(path, data.dump(1, '\t'));
}


//
//	OtInstances::add
//

void OtInstances::add(const glm::mat4 &instance, bool updateVersion) {
	instances->emplace_back(instance);

	if (updateVersion) {
	 	incrementVersion();
	}
}


//
//	OtInstances::getVisible
//

bool OtInstances::getVisible(OtCamera& camera, OtAABB& aabb, std::vector<glm::mat4>& visibleInstances) {
	if (instances->size()) {
		// filter instances based on visibility
		struct InstanceReference {
			InstanceReference(size_t i, float d) : index(i), distance(d) {}
			size_t index;
			glm::mat4 matrix;
			float distance;
		};

		std::vector<InstanceReference> instanceReferences;

		for (size_t i = 0; i < instances->size(); i++) {
			auto instanceAabb = aabb.transform(instances->at(i));

			if (camera.isVisibleAABB(instanceAabb)) {
				instanceReferences.emplace_back(i, glm::distance(camera.position, instanceAabb.getCenter()));
			}
		}

		if (instanceReferences.size()) {
			// sort instances by distance to camera
			std::sort(instanceReferences.begin(), instanceReferences.end(), [&](const InstanceReference& i1, const InstanceReference& i2) {
				return i1.distance < i2.distance;
			});

			// extract list of matrices
			visibleInstances.clear();

			for (auto& instanceReference : instanceReferences) {
				visibleInstances.emplace_back(instances->at(instanceReference.index));
			}

			return true;

		} else {
			return false;
		}

	} else {
		return false;
	}
}
