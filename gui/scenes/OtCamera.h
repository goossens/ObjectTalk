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

#include "OtController.h"
#include "OtFrustum.h"
#include "OtGui.h"
#include "OtRenderingContext.h"


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
	OtObject setNearFarLimits(float nearMin, float nearMax, float farMin, float farMax);

	OtObject setScriptControlMode();
	OtObject setCircleTargetMode();
	OtObject setFirstPersonMode();

	// change camera geometry
	OtObject setPosition(float x, float y, float z);
	OtObject setTarget(float x, float y, float z);
	OtObject setUp(float x, float y, float z);

	void setPositionVector(glm::vec3 position);
	void setTargetVector(glm::vec3 target);
	void setUpVector(glm::vec3 up);

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

	// update camera for next frame
	void update(float aspectRatio);

	// submit data to BGFX
	void submit(OtRenderingContext context);

	// access camera information
	glm::vec3 getPosition() { return cameraPosition; }
	glm::vec3 getTarget() { return cameraTarget; }
	glm::vec3 getUp() { return cameraUp; }
	float getFOV() { return fov; }
	float getWidth() { return width; }
	float getNearClip() { return near; }
	float getFarClip() { return far; }

	glm::mat4& getViewMatrix() { return viewMatrix; }
	glm::mat4& getProjectionMatrix() { return projMatrix; }
	glm::mat4& getViewProjectionMatrix() { return viewProjMatrix; }
	glm::mat4& getInverseViewProjectionMatrix() { return invViewProjMatrix; }

	// see if object is visible in frustum
	bool isVisiblePoint(const glm::vec3& point);
	bool isVisibleAABB(const glm::vec3& min, const glm::vec3& max);
	bool isVisibleSphere(const glm::vec3& center, float radius);

	// get direction from Normal Device Coordinates (NDC -1 to 1)
	glm::vec3 getDirectionFromNDC(float x, float y);

	// has camera changed?
	bool hasChanged() { return changed; }

	// GUI to change camera properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCamera create();
	static OtCamera create(OtCamera camera);

private:
	// camera style
	enum OtCameraStyle {
		perspectiveStyle,
		orthographicStyle
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

	float nearMin = 0.1;
	float nearMax = 10.0;
	float farMin = 10.0;
	float farMax = 2000.0;

	glm::vec3 forward;
	glm::vec3 right;

	// transformation matrices;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	glm::mat4 viewProjMatrix;
	glm::mat4 invViewProjMatrix;

	// our viewing frustum
	OtFrustum frustum;

	// has camera changed since last frame
	bool changed = true;
};


//
//	Controller widget
//

OT_CONTROLLER(Camera)
