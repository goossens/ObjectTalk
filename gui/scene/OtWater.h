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
#include "OtDynamicVertexBuffer.h"
#include "OtFrameBuffer.h"
#include "OtIndexBuffer.h"
#include "OtObject3d.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtTexture.h"
#include "OtUniform.h"


//
//	OtWaterClass
//

class OtWaterClass;
typedef std::shared_ptr<OtWaterClass> OtWater;

class OtWaterClass : public OtObject3dClass {
public:
	// constructor
	OtWaterClass();

	// set the size of the water object
	OtObject setSize(int size);

	// set normal map
	OtObject setNormalMap(const std::string& normals);

	// set the scale of the normals
	OtObject setNormalScale(float scale);

	// update state
	void update(OtRenderer& renderer) override;

	// submit to GPU
	void render(OtRenderer& renderer) override;

	void renderReflection(OtRenderer& renderer);
	void renderRefraction(OtRenderer& renderer);
	void renderWater(OtRenderer& renderer);

	// GUI to change water properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWater create();

protected:
	// normal map
	OtTexture normals;

	// size of water plane
	float size = 1000.0;

	// water properties
	glm::vec3 color = { 0.5, 0.6, 0.8 };
	float time = 0.0;
	float scale = 1.0;
	float shininess = 50.0;

	// rendering views
	OtPass reflectionPass;
	OtPass refractionPass;

	// Frame buffers
	OtFrameBuffer reflectionFrameBuffer = OtFrameBuffer(
		OtFrameBuffer::rgbaFloatTexture,
		OtFrameBuffer::dFloatTexture);

	OtFrameBuffer refractionFrameBuffer = OtFrameBuffer(
		OtFrameBuffer::rgbaFloatTexture,
		OtFrameBuffer::dFloatTexture);

	// vertex buffers
	glm::vec3 waterVertices[4];

	// GPU assets
	OtDynamicVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;
	OtUniform waterUniform = OtUniform("u_water", 2);
	OtSampler normalsSampler = OtSampler("s_normals");
	OtSampler reflectionSampler = OtSampler("s_reflection");
	OtSampler refractionSampler = OtSampler("s_refraction");
	OtShader shader = OtShader("OtWaterVS", "OtWaterFS");
};


//
//	Controller widget
//

OT_CONTROLLER(Water)
