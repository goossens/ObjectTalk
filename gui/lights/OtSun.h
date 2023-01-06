//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtController.h"
#include "OtDirectionalLight.h"


//
//	OtSunClass
//

class OtSunClass;
typedef std::shared_ptr<OtSunClass> OtSun;

class OtSunClass : public OtDirectionalLightClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setElevation(float elevation);
	OtObject setAzimuth(float azimuth);

	// get the direction yo and from the sun
	glm::vec3 getDirectionToSun();
	glm::vec3 getLightDirection();

	// add light properties to renderer
	void addPropertiesToRenderer(OtRenderer& renderer) override;

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
