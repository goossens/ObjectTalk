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

	// GUI to change camera properties
	virtual void renderGUI();

	// get type definition
	static OtType getMeta();

protected:
	// update camera
	void updateViewMatrix();
	void updateDerived();

	// properties
	glm::vec3 position = { 0.0, 0.0, 10.0 };
	glm::vec3 target = { 0.0, 0.0, 0.0 };
	glm::vec3 up = { 0.0, 1.0, 0.0 };

	glm::mat4 viewMatrix = glm::mat4(1.0);
	glm::mat4 projMatrix = glm::mat4(1.0);

	// derived products
	glm::mat4 viewProjMatrix = glm::mat4(1.0);
	OtFrustum frustum;
};
