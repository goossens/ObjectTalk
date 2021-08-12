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

#include "gui.h"


//
//	OtLightClass
//

class OtLightClass;
typedef std::shared_ptr<OtLightClass> OtLight;

class OtLightClass : public OtGuiClass {
	friend class OtViewClass;

public:
	// initialize light
	void init(bool a,
		double px, double py, double pz,
		double dr, double dg, double db,
		double sr, double sg, double sb) {
			active = a;
			position = glm::vec4(px, py, pz, 1.0);
			diffuse = glm::vec4(dr, dg, db, 1.0);
			specular = glm::vec4(sr, sg, sb, 1.0);
	}

	// update attributes
	void setPosition(double px, double py, double pz) { position = glm::vec4(px, py, pz, 1.0); }
	void setDiffuse(double dr, double dg, double db) { diffuse = glm::vec4(dr, dg, db, 1.0); }
	void setSpecular(double sr, double sg, double sb) { specular = glm::vec4(sr, sg, sb, 1.0); }

	// GUI to change fog parameters
	void renderGUI(int number);

	// submit data to BGFX
	void submit(glm::vec4* slot, const glm::mat4& viewMatrix);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtLight create();

private:
	// light properties
	bool active = false;
	glm::vec4 position;
	glm::vec4 diffuse;
	glm::vec4 specular;
};
