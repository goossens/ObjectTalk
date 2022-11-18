//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtGui.h"


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

	// initialize blendmap
	void init(const std::string& bm, const std::string& tn, const std::string& tr, const std::string& tg, const std::string& tb);

	// submit shader data to BGFX
	void submit();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBlendMap create();

private:
	// uniforms
	bgfx::UniformHandle blendmapUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformN = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformR = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformG = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniformB = BGFX_INVALID_HANDLE;

	// textures
	bgfx::TextureHandle blendmap = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureN = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureR = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureG = BGFX_INVALID_HANDLE;
	bgfx::TextureHandle textureB = BGFX_INVALID_HANDLE;
};
