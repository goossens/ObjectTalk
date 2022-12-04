//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtController.h"
#include "OtLight.h"


//
//	OtSunClass
//

class OtSunClass;
typedef std::shared_ptr<OtSunClass> OtSun;

class OtSunClass : public OtLightClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setElevation(float elevation);
	OtObject setAzimuth(float azimuth);
	OtObject castShadow(float width, float distance, float near, float far);

	// get the direction yo and from the sun
	glm::vec3 getDirectionToSun();
	glm::vec3 getLightDirection();

	// update state
	void update(OtRenderingContext context) override;

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

	// for shadow calculations
	float distance = 1.0;
};


//
//	Controller widget
//

OT_CONTROLLER(Sun)
