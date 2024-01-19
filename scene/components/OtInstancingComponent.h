//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtAABB.h"
#include "OtFrustum.h"

#include "OtTransformComponent.h"


//
//	OtInstancingComponent
//

class OtInstancingComponent {
public:
	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Instancing";

	// manipulate instances
	void clearInstances() { transforms.clear(); }
	void appendInstance(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);
	OtTransformComponent& getInstance(size_t index) { return transforms[index]; }
	void setInstance(size_t index, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);
	void eraseInstance(size_t index) { transforms.erase(transforms.begin() + index); }
	size_t instanceCount() { return transforms.size(); }

	// determine visible instances
	void determineVisibleInstances(OtFrustum& frustum, OtAABB& aabb);

	// stored properties
	std::vector<OtTransformComponent> transforms;

	// runtime properties
	std::vector<glm::mat4> visibleTransforms;
};
