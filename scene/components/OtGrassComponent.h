//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"


//
//	OtGrassComponent
//

class OtGrassComponent {
public:
	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Grass";

	// stored properties
	float patchWidth = 1.0f;
	float patchDepth = 1.0f;
	int blades = 10;
	int segments = 4;

	float bladeWidth = 0.1f;
	float bladeHeight = 1.0f;
	float bladePointiness = 0.5f;

	float widthVariation = 0.2f;
	float heightVariation = 0.2f;
	float colorVariation = 0.2f;

	glm::vec3 baseColor{0.1f, 0.4f, 0.04f};
	glm::vec3 tipColor{0.4f, 0.6f, 0.2f};
	bool castShadow = true;
};
