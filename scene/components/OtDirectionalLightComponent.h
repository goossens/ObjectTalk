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
//	OtDirectionalLightComponent
//

class OtDirectionalLightComponent {
public:
	// constructors
	OtDirectionalLightComponent() = default;
	OtDirectionalLightComponent(const glm::vec3& c) : color(c) {}

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Directional Light";

	// stored properties
	glm::vec3 color{1.0f};
};
