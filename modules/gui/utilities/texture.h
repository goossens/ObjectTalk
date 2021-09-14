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

#include "gui.h"


//
//	OtTexture
//

class OtTextureClass;
typedef std::shared_ptr<OtTextureClass> OtTexture;

class OtTextureClass : public OtGuiClass {
public:
	// destructor
	~OtTextureClass();

	// initialize
	void init(const std::string& texture);

	// submit shader data to BGFX
	void submit(int stage, bgfx::UniformHandle uniform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTexture create();

private:
	// texture properties
	bimg::ImageContainer* image = nullptr;
	bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
};
