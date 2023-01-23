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

#include "OtComponent.h"


//
//	OtTransformComponent
//

class OtTransformComponent : public OtComponent {
public:
	// constructors
	OtTransformComponent() = default;

	OtTransformComponent(const glm::vec3& t, const glm::vec3& r, const glm::vec3& s) :
		translation(t), rotation(r), scale(s), active(true) {}

	// return component name
	static const char* getName() { return "Transform"; }

	// get the full transform
	glm::mat4 getTransform() const;

	// reset the transform
	void reset();

	// GUI to change component properties
	bool renderGUI() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// properties
	glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	bool active = true;
};
