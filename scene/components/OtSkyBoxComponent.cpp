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

#include "OtUi.h"

#include "OtComponentTools.h"
#include "OtSkyBoxComponent.h"


//
//	OtSkyBoxComponent::renderGUI
//

bool OtSkyBoxComponent::renderGUI() {
	bool changed = false;

	changed |= OtUiFileSelector("Right", right);
	changed |= OtUiFileSelector("Left", left);
	changed |= OtUiFileSelector("Top", top);
	changed |= OtUiFileSelector("Bottom", bottom);
	changed |= OtUiFileSelector("Front", front);
	changed |= OtUiFileSelector("Back", back);

	if (changed) {
		update = true;
	}

	changed |= ImGui::SliderFloat("Brightness", &brightness, 0.1f, 4.0f, "%.2f");
	changed |= ImGui::SliderFloat("Gamma", &gamma, 0.1f, 4.0f, "%.2f");

	return changed;
}


//
//	OtSkyBoxComponent::serialize
//

nlohmann::json OtSkyBoxComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["right"] = OtComponentGetRelativePath(right, basedir);
	data["left"] = OtComponentGetRelativePath(left, basedir);
	data["top"] = OtComponentGetRelativePath(top, basedir);
	data["bottom"] = OtComponentGetRelativePath(bottom, basedir);
	data["front"] = OtComponentGetRelativePath(front, basedir);
	data["back"] = OtComponentGetRelativePath(back, basedir);
	data["brightness"] = brightness;
	data["gamma"] = gamma;
	return data;
}


//
//	OtSkyBoxComponent::deserialize
//

static inline bool didPathChange(std::filesystem::path& value, nlohmann::json data, const char* name, std::filesystem::path* basedir) {
	auto oldValue = value;
	value = OtComponentGetAbsolutePath(data, name, basedir);
	return value != oldValue;
}

void OtSkyBoxComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	update |= didPathChange(right, data, "right", basedir);
	update |= didPathChange(left, data, "left", basedir);
	update |= didPathChange(top, data, "top", basedir);
	update |= didPathChange(bottom, data, "bottom", basedir);
	update |= didPathChange(front, data, "front", basedir);
	update |= didPathChange(back, data, "back", basedir);
	brightness = data.value("brightness", 1.0f);
	gamma = data.value("gamma", 2.2f);
}


//
//	OtSkyBoxComponent::isValid
//

bool OtSkyBoxComponent::isValid() {
	// update the cubemap (if required)
	if (update) {
		update = false;

		// wait until all sides have valid files
		if (std::filesystem::is_regular_file(right) &&
			std::filesystem::is_regular_file(left) &&
			std::filesystem::is_regular_file(top) &&
			std::filesystem::is_regular_file(bottom) &&
			std::filesystem::is_regular_file(front) &&
			std::filesystem::is_regular_file(back)) {

			// load the files into the cubemap
			cubemap.load(right, left, top, bottom, front, back);

		} else {
			// invalidate cubemap just in case
			cubemap.clear();
		}
	}

	return cubemap.isValid();
}
