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
#include "OtLight.h"


//
//	OtAmbientLightClass
//

class OtAmbientLightClass;
typedef std::shared_ptr<OtAmbientLightClass> OtAmbientLight;

class OtAmbientLightClass : public OtLightClass {
	friend class OtViewClass;

public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);

	void setColorVector(const glm::vec3 c) { color = c; }

	// add light properties to renderer
	void addPropertiesToRenderer(OtRenderer& renderer) override;

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtAmbientLight create();

private:
	// light properties
	glm::vec3 color = glm::vec3(1.0);
};


//
//	Controller widget
//

OT_CONTROLLER(AmbientLight)
