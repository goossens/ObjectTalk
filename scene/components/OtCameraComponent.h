//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"


//
//	OtCameraComponent
//

class OtCameraComponent {
public:
	// get the matrix
	glm::mat4 getProjectionMatrix(float aspectRatio);

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Camera";

	// stored properties
	float fov = 60.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
};
