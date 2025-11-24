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
//	OtDirectionalLightComponent
//

class OtDirectionalLightComponent {
public:
	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Directional Light";

	// get the direction to the light
	glm::vec3 getDirectionToLight();

	// location of the light
	float elevation = 25.0f;	// in degrees from XZ plane (positive is up)
	float azimuth = 180.0f;		// in clockwise degrees from negative Z axis
								// 0 = "north", 90 = "east", 180 = "south", 270 = "west"

	// stored properties
	glm::vec3 color{1.0f};
	float ambient = 0.05f;
	bool castShadow = false;
};
