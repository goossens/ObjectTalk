//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bimg/bimg.h"

#include "OtSceneObject.h"


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
	OtObject setCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz);

	// render in BGFX
	void render(OtRenderingContext context) override;

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
	bgfx::TextureHandle cubemap = BGFX_INVALID_HANDLE;

	bimg::ImageContainer* posxImage = nullptr;
	bimg::ImageContainer* negxImage = nullptr;
	bimg::ImageContainer* posyImage = nullptr;
	bimg::ImageContainer* negyImage = nullptr;
	bimg::ImageContainer* poszImage = nullptr;
	bimg::ImageContainer* negzImage = nullptr;
};
