//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "bimg/decode.h"

#include "sceneobject.h"


//
//	OtSkyboxClass
//

class OtSkyboxClass;
typedef std::shared_ptr<OtSkyboxClass> OtSkybox;

class OtSkyboxClass : public OtSceneObjectClass {
public:
	// constructor/destructor
	OtSkyboxClass();
	~OtSkyboxClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// set the cubemap for the skybox
	OtObject setCubemap(const std::string& file);

	// render in BGFX
    void render(int view, OtCamera camera, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSkybox create();

protected:
	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;

	// BGFX shader
	bgfx::UniformHandle transformUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;

	// our cubemap
	bgfx::UniformHandle cubemapUniform = BGFX_INVALID_HANDLE;
	bimg::ImageContainer* image = nullptr;
	bgfx::TextureHandle cubemap = BGFX_INVALID_HANDLE;
};
