//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "controller.h"
#include "sceneobject.h"


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

	// GUI to change ambient properties
	void renderGUI();

	// submit data to BGFX
	void submit(glm::vec4* slot);

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
