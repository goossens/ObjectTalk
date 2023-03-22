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

#include "OtException.h"

#include "OtUi.h"

#include "OtPathTools.h"
#include "OtModelComponent.h"


//
//	OtModelComponent::renderGUI
//

bool OtModelComponent::renderGUI() {
	bool changed = false;
	changed |= OtUiFileSelector("Model", modelPath);

	if (changed) {
		update();
	}

	return changed;
}


//
//	OtModelComponent::serialize
//

nlohmann::json OtModelComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["modelPath"] = OtPathGetRelative(modelPath, basedir);
	return data;
}


//
//	OtModelComponent::deserialize
//

void OtModelComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	modelPath = OtPathGetAbsolute(data, "modelPath", basedir);
	update();
}


//
//	OtModelComponent::update
//

void OtModelComponent::update() {
	// update the model
	if (std::filesystem::is_regular_file(modelPath)) {
		model.load(modelPath);

	} else {
		model.clear();
	}
}
