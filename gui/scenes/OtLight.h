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
//	OtLightClass
//

class OtLightClass;
typedef std::shared_ptr<OtLightClass> OtLight;

class OtLightClass : public OtSceneObjectClass {
public:
	// update attributes
	OtObject setPosition(float x, float y, float z);
	OtObject setDiffuse(const std::string& color) ;
	OtObject setSpecular(const std::string& color);

	// update state
	void update(OtRenderingContext context);

	// GUI to change light properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtLight create();

private:
	// light properties
	glm::vec3 position = glm::vec3(0.0);
	glm::vec3 diffuse = glm::vec3(1.0);
	glm::vec3 specular = glm::vec3(1.0);
};


//
//	Controller widget
//

OT_CONTROLLER(Light)
