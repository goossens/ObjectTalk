//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCubeMap.h"
#include "OtIndexBuffer.h"
#include "OtSampler.h"
#include "OtSceneObject.h"
#include "OtShader.h"
#include "OtVertexBuffer.h"


//
//	OtSkyboxClass
//

class OtSkyboxClass;
typedef std::shared_ptr<OtSkyboxClass> OtSkybox;

class OtSkyboxClass : public OtSceneObjectClass {
public:
	// constructor
	OtSkyboxClass();

	// initialize
	void init(size_t count, OtObject* parameters);

	// set the cubemap for the skybox
	OtObject setCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz);

	// submit to GPU
	void render(OtRenderer& renderer) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSkybox create();

protected:
	// GPU assets
	OtVertexBuffer vertexBuffer;
	OtIndexBuffer indexBuffer;
	OtCubeMap cubemap;
	OtSampler cubemapSampler = OtSampler("s_cubemap");
	OtShader shader = OtShader("OtSkyboxVS", "OtSkyboxFS");
};
