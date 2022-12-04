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

#include "OtNumbers.h"

#include "OtController.h"
#include "OtLight.h"


//
//	OtSpotLightClass
//

class OtSpotLightClass;
typedef std::shared_ptr<OtSpotLightClass> OtSpotLight;

class OtSpotLightClass : public OtLightClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setPosition(float x, float y, float z);
	OtObject setDirection(float x, float y, float z);
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);
	OtObject setAttenuation(float constant, float linear, float quadratic);
	OtObject setCone(float inner, float outer);

	void setPositionVector(const glm::vec3 p) { position = p; }
	void setDirectionVector(const glm::vec3 d) { direction = d; }
	void setColorVector(const glm::vec3 c) { color = c; }
	void setAttenuationVector(const glm::vec3 a) { attenuation = a; }

	// update state
	void update(OtRenderingContext context) override;

	// GUI to change ambient properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSpotLight create();

private:
	// properties
	glm::vec3 position = glm::vec3(0.0, 0.0, 1.0);
	glm::vec3 direction = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 color = glm::vec3(1.0);
	glm::vec3 attenuation = glm::vec3(1.0, 0.1, 0.1);
	float inner = std::numbers::pi / 2.0;
	float outer = std::numbers::pi / 2.0;
};


//
//	Controller widget
//

OT_CONTROLLER(SpotLight)
