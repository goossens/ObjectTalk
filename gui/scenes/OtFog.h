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
#include "OtSceneObject.h"


//
//	OtFogClass
//

class OtFogClass;
typedef std::shared_ptr<OtFogClass> OtFog;

class OtFogClass : public OtSceneObjectClass {
	friend class OtViewClass;

public:
	// update attributes
	OtObject setColor(const std::string& color);
	OtObject setDistances(float near, float far);
	OtObject setLimits(float minNear, float maxNear, float minFar, float maxFar);

	// GUI to change fog properties
	void renderGUI();

	// submit data to BGFX
	void render(OtRenderingContext* context);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFog create();

private:
	// fog properties
	glm::vec3 color = glm::vec4(1.0);
	float near = 0;
	float far = 100;

	float minNear = 0;
	float maxNear = 10;

	float minFar = 50;
	float maxFar = 150;
};


//
//	Controller widget
//

OT_CONTROLLER(Fog)
