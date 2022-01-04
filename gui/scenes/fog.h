//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "controller.h"
#include "sceneobject.h"


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
};


//
//	Controller widget
//

OT_CONTROLLER(Fog)
