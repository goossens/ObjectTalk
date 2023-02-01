//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtComponentTools.h"
#include "OtSkySphereComponent.h"


//
//	OtSkySphereComponent::renderGUI
//


bool OtSkySphereComponent::renderGUI() {
	bool changed = false;

	if (OtUiFileSelector("Image", image)) {
		changed = true;
		update = true;
	}

	return changed;
}


//
//	OtSkySphereComponent::serialize
//

nlohmann::json OtSkySphereComponent::serialize(std::filesystem::path* basedir) {
	auto data = nlohmann::json::object();
	data["component"] = name;
	data["image"] = OtComponentGetRelativePath(image, basedir);
	return data;
}


//
//	OtSkySphereComponent::deserialize
//

void OtSkySphereComponent::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	image = OtComponentGetAbsolutePath(data, "image", basedir);
	update = true;
}


//
//	OtSkySphereComponent::isValid
//

bool OtSkySphereComponent::isValid() {
	// update the texture (if required)
	if (update) {
		update = false;

		// wait until we have a valid file
		if (std::filesystem::is_regular_file(image)) {
			texture.loadFromFile(image.string());

		} else {
			// invalidate texture just in case
			texture.clear();
		}
	}

	return texture.isValid();
}
