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
//	OtSkyComponent
//

class OtSkyComponent {
public:
	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Sky";

	// get the direction to the sun
	glm::vec3 getDirectionToSun();

	// location of the sun
	float elevation = 25.0f;	// in degrees from XZ plane (positive is up)
	float azimuth = 180.0f;		// in clockwise degrees from negative Z axis
								// 0 = "north", 90 = "east", 180 = "south", 270 = "west"

	// shadow flag
	bool castShadow = true;

	// atmospheric scattering
	float rayleighCoefficient = 3.5f;
	float mieCoefficient = 5.0f;
	float mieScattering =  0.99f;

	// cloud settings
	float cirrus = 0.4f;
	float cumulus = 0.8f;
	float speed = 2.0f;
};
