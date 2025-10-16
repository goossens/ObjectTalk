//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"


//
//	OtGrass
//

class OtGrass {
public:
	// UI to change grass properties
	bool renderUI();

	// (de)serialize material
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// submit grass geometry
	void submit();

	// grass properties
	float patchWidth = 1.0f;
	float patchDepth = 1.0f;
	int blades = 10;

	int bladeSegments = 4;
	float bladeWidth = 0.1f;
	float bladeHeight = 1.0f;
	float bladePointiness = 0.5f;
	float bladeCurve = 0.5f;

	float windDirection = 0.0f;
	float windStrength = 0.2f;

	glm::vec3 baseColor{0.1f, 0.4f, 0.04f};
	glm::vec3 tipColor{0.4f, 0.6f, 0.2f};

	float widthVariation = 0.2f;
	float heightVariation = 0.2f;
	float windVariation = 0.2f;
	float colorVariation = 0.2f;

	bool castShadow = true;
};
