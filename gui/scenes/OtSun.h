//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCamera.h"
#include "OtController.h"
#include "OtSceneObject.h"


//
//	OtSunClass
//

class OtSunClass;
typedef std::shared_ptr<OtSunClass> OtSun;

class OtSunClass : public OtSceneObjectClass {
	friend class OtViewClass;

public:
	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setElevation(float elevation);
	OtObject setAzimuth(float azimuth);

	// get the direction towards the sun
	glm::vec3 getDirection();

	// get ambient light emitted by sun
	float getAmbientLight();

	// update state
	void update(OtRenderingContext context);

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSun create();

private:
	// location of the sun
	float elevation = 0.0;	// in radians from XZ plane (positive is up)
	float azimuth = 0.0;	// in clockwise radians from negative Z axis
							// 0 = "north", 1/2 pi = "east", pi = "south", 1 1/2 pi = "west"
};


//
//	Controller widget
//

OT_CONTROLLER(Sun)
