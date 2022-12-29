//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtNumbers.h"

#include "OtAABB.h"
#include "OtController.h"
#include "OtFrustum.h"
#include "OtGui.h"


//
//	OtCameraClass
//

class OtCameraClass;
typedef std::shared_ptr<OtCameraClass> OtCamera;

class OtCameraClass : public OtGuiClass {
public:
	// set camera modes
	OtObject setPerspective(float fov, float near, float far);
	OtObject setOrthographic(float width, float near, float far);
	OtObject setOrthographicCustom(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);

	OtObject setFovLimits(float fovMin, float fovMax);
	OtObject setWidthLimits(float widthMin, float widthMax);
	OtObject setNearFarLimits(float nearMin, float nearMax, float farMin, float farMax);

	OtObject setScriptControlMode();
	OtObject setCircleTargetMode();
	OtObject setFirstPersonMode();

	// change camera geometry
	OtObject setPosition(float x, float y, float z);
	OtObject setTarget(float x, float y, float z);
	OtObject setUp(float x, float y, float z);

	void setPositionVector(const glm::vec3& position);
	void setTargetVector(const glm::vec3& target);
	void setUpVector(const glm::vec3& up);
	void setAspectRatio(float aspectRatio);

	// set properties
	OtObject setDistance(float distance);
	OtObject setPitch(float pitch);
	OtObject setYaw(float yaw);
	OtObject setDistanceLimits(float min, float max);
	OtObject setPitchLimits(float min, float max);
	OtObject setYawLimits(float min, float max);
	OtObject setHeightLimits(float min, float max);

	// mouse/keyboard events
	bool onMouseDrag(int button, int mods, float xpos, float ypos);
	bool onScrollWheel(float dx, float dy);
	bool onKey(int key, int mods);

	// access camera information
	glm::vec3 getPosition() { validate(); return cameraPosition; }
	glm::vec3 getTarget() { validate(); return cameraTarget; }
	glm::vec3 getUp() { validate(); return cameraUp; }
	float getFOV() { validate(); return fov; }
	float getWidth() { validate(); return width; }
	float getNearClip() { validate(); return near; }
	float getFarClip() { validate(); return far; }
	float getAspectRatio() { validate(); return aspectRatio; }

	glm::mat4& getViewMatrix() { validate(); return viewMatrix; }
	glm::mat4& getProjectionMatrix() { validate(); return projMatrix; }
	glm::mat4& getViewProjectionMatrix() { validate(); return viewProjMatrix; }
	OtFrustum getFrustum() { validate(); return frustum; }

	// see if object is visible in frustum
	bool isVisiblePoint(const glm::vec3& point);
	bool isVisibleAABB(OtAABB aabb);
	bool isVisibleSphere(const glm::vec3& center, float radius);

	// GUI to change camera properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCamera create();
	static OtCamera create(OtCamera camera);

private:
	// validate camera properties and update if required
	inline void validate() {
		if (changed) {
			update();
			changed = false;
		}
	}

	void update();

	// camera style
	enum OtCameraStyle {
		perspectiveStyle,
		orthographicStyle,
		orthographicCustomStyle
	};

	OtCameraStyle style = perspectiveStyle;

	// camera mode
	enum OtCameraMode {
		scriptControlMode,
		circleTargetMode,
		firstPersonMode
	};

	OtCameraMode mode = scriptControlMode;

	// camera geometry
	glm::vec3 cameraPosition = { 0.0, 0.0, 10.0 };
	glm::vec3 cameraTarget = { 0.0, 0.0, 0.0 };
	glm::vec3 cameraUp = { 0.0, 1.0, 0.0 };

	// camera properties
	float distance = 10.0;
	float pitch = 0.0;
	float yaw = 0.0;

	float distanceMin = 0.0;
	float distanceMax = 100.0;
	float pitchMin = -std::numbers::pi / 2.0 + 0.01;
	float pitchMax = std::numbers::pi / 2.0 - 0.01;
	float yawMin = -std::numbers::pi;
	float yawMax = std::numbers::pi;
	float heightMin = 0.0;
	float heightMax = 1000.0;

	float fov = 60.0;
	float width = 100.0;
	float near = 0.1;
	float far = 2000.0;

	float xmin;
	float xmax;
	float ymin;
	float ymax;
	float zmin;
	float zmax;

	float fovMin = 10.0;
	float fovMax = 120.0;
	float widthMin = 10.0;
	float widthMax = 1000.0;
	float nearMin = 0.1;
	float nearMax = 10.0;
	float farMin = 10.0;
	float farMax = 2000.0;

	glm::vec3 forward;
	glm::vec3 right;
	float aspectRatio = 1.0;

	// transformation matrices;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::mat4 viewProjMatrix;

	// our viewing frustum
	OtFrustum frustum;

	// has camera changed since last frame
	bool changed = true;
};


//
//	Controller widget
//

OT_CONTROLLER(Camera)
