//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtInstancingComponent.h"


//
//	OtInstancingComponent::renderGUI
//

bool OtInstancingComponent::renderGUI() {
	return false;
}


//
//	OtInstancingComponent::serialize
//

nlohmann::json OtInstancingComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;

	auto instances = nlohmann::json::array();

	for (auto& transform : transforms) {
		instances.push_back(transform.serialize(basedir));
	}

	return data;
}


//
//	OtInstancingComponent::deserialize
//

void OtInstancingComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	transforms.clear();

	for (auto& transform : data) {
		transforms.emplace_back().deserialize(transform, basedir);
	}
}


//
//	OtInstancingComponent::appendInstance
//

void OtInstancingComponent::appendInstance(const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale) {
	auto& transform = transforms.emplace_back();
	transform.translation = translation;
	transform.rotation = rotation;
	transform.scale = scale;
}


//
//	OtInstancingComponent::setInstance
//

void OtInstancingComponent::setInstance(size_t index, const glm::vec3 &translation, const glm::vec3 &rotation, const glm::vec3 &scale) {
	auto& transform = transforms[index];
	transform.translation = translation;
	transform.rotation = rotation;
	transform.scale = scale;
}


//
//	OtInstancingComponent::determineVisibleInstances
//

void OtInstancingComponent::determineVisibleInstances(OtFrustum& frustum, OtAABB& aabb) {
	visibleTransforms.clear();

	for (auto& transform : transforms) {
		auto matrix = transform.getTransform();

		if (frustum.isVisibleAABB(aabb.transform(matrix))) {
			visibleTransforms.emplace_back(matrix);
		}
	}
}
