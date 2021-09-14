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

#include "sceneobject.h"


//
//	OtLightClass
//

class OtLightClass;
typedef std::shared_ptr<OtLightClass> OtLight;

class OtLightClass : public OtSceneObjectClass {
public:
	// update attributes
	OtObject setPosition(double x, double y, double z);
	OtObject setDiffuse(const std::string c) ;
	OtObject setSpecular(const std::string c);

	// GUI to change parameters
	void renderGUI();

	// submit data to BGFX
	void submit(glm::vec4* slot, const glm::mat4& viewMatrix);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtLight create();

private:
	// light properties
	glm::vec4 position = glm::vec4(0.0);
	glm::vec4 diffuse = glm::vec4(1.0);
	glm::vec4 specular = glm::vec4(1.0);
};
