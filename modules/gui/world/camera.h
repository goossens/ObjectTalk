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
	friend class OtViewClass;

public:
	// change camera geometry
	OtObject setPosition(double x, double y, double z);
	OtObject setDirection(double x, double y, double z);
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

	// GUI to change camera parameters
	void renderGUI();

	// submit data to BGFX
	void submit(int view, float viewAspect);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCamera create();

private:
	// camera geometry
	glm::vec3 cameraPos = { 0.0, 0.0, 10.0 };
	glm::vec3 cameraDir = { 0.0, 0.0, 0.0 };
	glm::vec3 cameraUp = { 0.0, 1.0, 0.0 };

	// mouse control
	bool mouseControl = false;
	glm::vec3 target = { 0.0, 0.0, 0.0 };
	double distance = 10.0;
	double angle = 0.0;
	double pitch = 0.3;

	double distanceMin = 0.0;
	double distanceMax = 100.0;
	double angleMin = -std::numbers::pi;
	double angleMax = std::numbers::pi;
	double pitchMin = -std::numbers::pi / 2.0;
	double pitchMax = std::numbers::pi / 2.0;

	// field-of-view and clipping
	float fov = 60.0;
	float nearClip = 0.1;
	float farClip = 10000.0;

	// transformation matrices;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
};
