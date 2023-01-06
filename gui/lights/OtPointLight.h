//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
//	OtPointLightClass
//

class OtPointLightClass;
typedef std::shared_ptr<OtPointLightClass> OtPointLight;

class OtPointLightClass : public OtLightClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setPosition(float x, float y, float z);
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);
	OtObject setAttenuation(float constant, float linear, float quadratic);

	void setPositionVector(const glm::vec3 p) { position = p; }
	void setColorVector(const glm::vec3 c) { color = c; }
	void setAttenuationVector(const glm::vec3 a) { attenuation = a; }

	// add light properties to renderer
	void addPropertiesToRenderer(OtRenderer& renderer) override;

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPointLight create();

private:
	// properties
	glm::vec3 position = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 color = glm::vec3(1.0);
	glm::vec3 attenuation = glm::vec3(1.0, 0.1, 0.1);
};


//
//	Controller widget
//

OT_CONTROLLER(PointLight)
