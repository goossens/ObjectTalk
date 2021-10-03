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

#include "bgfx/bgfx.h"

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

	// GUI to change fog parameters
	void renderGUI();

	// submit data to BGFX
	void submit(glm::vec4* slot);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFog create();

private:
	// fog properties
	glm::vec4 color = glm::vec4(1.0);
	float near = 0;
	float far = 100;
};
