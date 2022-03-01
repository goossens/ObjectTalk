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
//	OtAmbientClass
//

class OtAmbientClass;
typedef std::shared_ptr<OtAmbientClass> OtAmbient;

class OtAmbientClass : public OtSceneObjectClass {
	friend class OtViewClass;

public:
	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setColor(const std::string& color);
	OtObject setColorVector(const glm::vec3 color);

	// update state
	void update(OtRenderingContext context);

	// GUI to change ambient properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtAmbient create();

private:
	// light properties
	glm::vec3 color = glm::vec3(1.0);
};


//
//	Controller widget
//

OT_CONTROLLER(Ambient)
