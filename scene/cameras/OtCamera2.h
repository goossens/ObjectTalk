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

#include "OtFrustum.h"
#include "OtGui.h"


//
//	Abstract camera class
//

class OtCamera2Class;
typedef std::shared_ptr<OtCamera2Class> OtCamera2;

class OtCamera2Class : public OtGuiClass {
public:
	// setters
	void setPosition(const glm::vec3& p) { position = p; updateViewMatrix(); }
	void setTarget(const glm::vec3& t) { target = t; updateViewMatrix(); }
	void setUp(const glm::vec3& u) { up = u; updateViewMatrix(); }

	// getters
	glm::vec3& getPosition() { return position; }
	glm::vec3& getTarget() { return target; }
	glm::vec3& getUp() { return up; }

	glm::mat4& getViewMatrix() { return viewMatrix; }
	glm::mat4& getProjectionMatrix() { return projMatrix; }
	glm::mat4& getViewProjectionMatrix() { return viewProjMatrix; }
	OtFrustum& getFrustum() { return frustum; }

	virtual void setAspectRatio(float aspectRatio) {}

	// GUI to change camera properties
	virtual bool renderGUI();

	// mouse/keyboard events
	virtual void handleMouseKeyboard() {}

	// (de)serialize camera
	virtual nlohmann::json serialize();
	virtual void deserialize(nlohmann::json data);

	std::string serializeToString();
	void deserializeFromString(const std::string& data);

	// get type name of camera
	virtual const char* getTypeName() { return name; }

	// camera name
	static constexpr char const* name = "Camera";

	// get type definition
	static OtType getMeta();

protected:
	// update camera
	void updateViewMatrix();
	void updateDerived();

	// properties
	glm::vec3 position = { 0.0f, 0.0f, 10.0f };
	glm::vec3 target = { 0.0f, 0.0f, 0.0f };
	glm::vec3 up = { 0.0f, 1.0f, 0.0f };

	// derived products
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	glm::mat4 projMatrix = glm::mat4(1.0f);
	glm::mat4 viewProjMatrix = glm::mat4(1.0f);
	OtFrustum frustum;
};
