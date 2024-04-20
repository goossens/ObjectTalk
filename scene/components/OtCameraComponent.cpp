//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtCameraComponent.h"


//
//	OtCameraComponent::renderUI
//

bool OtCameraComponent::renderUI() {
	bool changed = false;
	changed |= OtUiToggleButton("Main Camera", &mainCamera);
	changed |= OtUiDragFloat("FoV (Deg)", &fov, 10.0f, 160.0f);
	changed |= OtUiDragFloat("Near Plane", &nearPlane, 0.001f);
	changed |= OtUiDragFloat("Far Plane", &farPlane, nearPlane);
	return changed;
}


//
//	OtCameraComponent::serialize
//

nlohmann::json OtCameraComponent::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["mainCamera"] = mainCamera;
	data["fov"] = fov;
	data["near"] = nearPlane;
	data["far"] = farPlane;
	return data;
}


//
//	OtCameraComponent::deserialize
//

void OtCameraComponent::deserialize(nlohmann::json data, std::string* basedir) {
	mainCamera = data.value("mainCamera", false);
	fov = data.value("fov", 60.0f);
	nearPlane = data.value("near", 0.1f);
	farPlane = data.value("far", 100.0f);
}
