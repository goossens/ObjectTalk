//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtPostProcessingComponent
//

class OtPostProcessingComponent {
public:
	// constructor
	OtPostProcessingComponent() = default;

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Post Processing";

	// stored properties
	float exposure = 1.0f;
	bool fogEnabled = false;
	glm::vec3 fogColor{1.0f};
	float fogNear = 0.1f;
	float fogFar = 1000.0f;
};
