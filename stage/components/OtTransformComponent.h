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
//	OtTransformComponent
//

class OtTransformComponent {
public:
	// constructors
	OtTransformComponent() = default;

	OtTransformComponent(const glm::vec3& t, const glm::vec3& r, const glm::vec3& s) :
		translation(t), rotation(r), scale(s), active(true) {}

	// get the full transform
	glm::mat4 getTransform() const;

	// reset the transform
	void reset();

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize();
	void deserialize(nlohmann::json data);

	// component name
	static constexpr char const* name = "Transform";

	// properties
	glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	bool active = true;
};
