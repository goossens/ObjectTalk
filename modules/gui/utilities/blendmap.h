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

#include "gui.h"
#include "texture.h"


//
//	OtBlendMapClass
//

class OtBlendMapClass;
typedef std::shared_ptr<OtBlendMapClass> OtBlendMap;

class OtBlendMapClass : public OtGuiClass {
public:
	// constructor/destructor
	OtBlendMapClass();
	~OtBlendMapClass();

	// initialize heightmap
	void init(OtObject blendmap, OtObject textureN, OtObject textureR, OtObject textureG, OtObject textureB);

	// submit shader data to BGFX
	void submit();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBlendMap create();

private:
	// validate and cast object to a texture
	OtTexture toTexture(OtObject object);

	// textures
	OtTexture blendmap;
	OtTexture textureN;
	OtTexture textureR;
	OtTexture textureG;
	OtTexture textureB;

	// uniforms
	bgfx::UniformHandle blendmapUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformN = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformR = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformG = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformB = BGFX_INVALID_HANDLE;
};
