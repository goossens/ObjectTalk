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

#include "OtController.h"
#include "OtSceneObject.h"
#include "OtSun.h"


//
//	OtSkyClass
//

class OtSkyClass;
typedef std::shared_ptr<OtSkyClass> OtSky;

class OtSkyClass : public OtSceneObjectClass {
public:
	// constructor/destructor
	OtSkyClass();
	~OtSkyClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// connect to sun object
	OtObject setSun(OtObject sun);

	// set the cloud density
	OtObject setClouds(float cirrus, float cumulus);

	// GUI to change sky properties
	void renderGUI();

	// render in BGFX
	void render(OtRenderingContext* context);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSky create();

protected:
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
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> triangles;

	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;

	// BGFX shader
	bgfx::UniformHandle skyUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};


//
//	Controller widget
//

OT_CONTROLLER(Sky)
