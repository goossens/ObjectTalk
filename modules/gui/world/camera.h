//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/ext.hpp"
#include "glm/glm.hpp"

#include "ot/numbers.h"

#include "gui.h"


//
//	OtCameraClass
//

class OtCameraClass;
typedef std::shared_ptr<OtCameraClass> OtCamera;

class OtCameraClass : public OtGuiClass {
public:
	// change camera geometry
	OtObject setPosition(double x, double y, double z);
	OtObject setTarget(double x, double y, double z);
	OtObject setUp(double x, double y, double z);
	OtObject setFOV(double fov);
	OtObject setClipping(double near, double far);

	// enable/disable camera mouse control
	OtObject setMouseControl(bool control);
	OtObject setDistance(double distance);
	OtObject setAngle(double angle);
	OtObject setPitch(double pitch);
	OtObject setDistanceLimits(double min, double max);
	OtObject setAngleLimits(double min, double max);
	OtObject setPitchLimits(double min, double max);

	// mouse events
	bool onMouseDrag(int button, int mods, double xpos, double ypos);
	bool onScrollWheel(double dx, double dy);

	// submit data to BGFX
	void submit(int view, float viewAspect);

	// access camera information
	glm::vec3 getPosition() { return cameraPosition; }
	glm::vec3 getTarget() { return cameraTarget; }
	glm::vec3 getUp() { return cameraUp; }

	glm::mat4& getViewMatrix() { return viewMatrix; }
	glm::mat4& getProjectionMatrix() { return projMatrix; }

	// see if AABB object is visible in frustum
	bool isVisiblePoint(const glm::vec3& point);
	bool isVisibleAABB(const glm::vec3& min, const glm::vec3& max);
	bool isVisibleSphere(const glm::vec3& center, double radius);

	// GUI to change camera parameters
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCamera create();

private:
	// camera geometry
	glm::vec3 cameraPosition = { 0.0, 0.0, 10.0 };
	glm::vec3 cameraTarget = { 0.0, 0.0, 0.0 };
	glm::vec3 cameraUp = { 0.0, 1.0, 0.0 };

	// mouse control
	bool mouseControl = false;
	glm::vec3 target = { 0.0, 0.0, 0.0 };
	float distance = 10.0;
	float angle = 0.0;
	float pitch = 0.3;

	float distanceMin = 0.0;
	float distanceMax = 100.0;
	float angleMin = -std::numbers::pi;
	float angleMax = std::numbers::pi;
	float pitchMin = -std::numbers::pi / 2.0;
	float pitchMax = std::numbers::pi / 2.0;

	// field-of-view and clipping
	float fov = 60.0;
	float nearClip = 0.1;
	float farClip = 10000.0;

	// transformation matrices;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	// a plane in our frustum
	class Plane {
	public:
		// constructors
		Plane() = default;

		Plane(const glm::vec4& v) : normal(v.x, v.y, v.z), d(v.w) {
			// normalize vector
			float length = glm::length(normal);
			normal /= length;
			d /= length;
		}

		glm::vec3 normal;
		float d;
	};

	// our viewing frustum
	Plane planes[6];
};
