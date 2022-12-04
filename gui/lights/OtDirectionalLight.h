//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtController.h"
#include "OtLight.h"


//
//	OtDirectionalLightClass
//

class OtDirectionalLightClass;
typedef std::shared_ptr<OtDirectionalLightClass> OtDirectionalLight;

class OtDirectionalLightClass : public OtLightClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setDirection(float x, float y, float z);
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);

	void setDirectionVector(const glm::vec3 d) { direction = d; }
	void setColorVector(const glm::vec3 c) { color = c; }

	// enable shadow casting
	OtObject castShadow(float width, float near, float far);

	// update state
	void update(OtRenderingContext context) override;

	// GUI to change ambient properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtDirectionalLight create();

private:
	// properties
	glm::vec3 direction = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 color = glm::vec3(1.0);
};


//
//	Controller widget
//

OT_CONTROLLER(DirectionalLight)
