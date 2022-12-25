//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"

#include "OtController.h"
#include "OtIndexBuffer.h"
#include "OtSceneObject.h"
#include "OtShader.h"
#include "OtSun.h"
#include "OtUniformVec4.h"
#include "OtVertexBuffer.h"


//
//	OtSkyClass
//

class OtSkyClass;
typedef std::shared_ptr<OtSkyClass> OtSky;

class OtSkyClass : public OtSceneObjectClass {
public:
	// constructor
	OtSkyClass();

	// initialize
	void init(size_t count, OtObject* parameters);

	// set the static sky color
	OtObject setColor(const std::string& color);
	OtObject setColorRGB(float r, float g, float b);
	void setColorVector(const glm::vec3& c) { color = c; }

	// connect to sun object
	OtObject setSun(OtObject sun);

	// set the cloud density
	OtObject setClouds(float cirrus, float cumulus);

	// set radius of sky dome
	OtObject setRadius(float radius);

	// GUI to change sky properties
	void renderGUI();

	// submit to GPU
	void render(OtRenderer& renderer) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSky create();

protected:
	// sky color (only used if no sun is specified)
	glm::vec3 color = glm::vec3(0.2, 0.6, 0.8);

	// sun reference
	OtSun sun;

	// atmospheric scattering
	float rayleighCoefficient = 3.5;
	float mieCoefficient = 5.0;
	float mieScattering =  0.99;

	// cloud variables
	float cirrus = 0.4;
	float cumulus = 0.8;

	// sky dome
	float radius = 1.0;
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> triangles;

	// function to create the skyd ome
	void createSkyDome();

	// GPU assets
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;
	OtUniformVec4 skyUniform = OtUniformVec4("u_sky", 3);
	OtUniformVec4 colorUniform = OtUniformVec4("u_material", 1);

	OtShader skyShader = OtShader("OtSkyVS", "OtSkyFS");
	OtShader colorShader = OtShader("OtFixedVS", "OtFixedFS");
};


//
//	Controller widget
//

OT_CONTROLLER(Sky)
