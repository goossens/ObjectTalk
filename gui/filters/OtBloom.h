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

#include "OtFilter.h"


//
//	OtBloomClass
//

class OtBloomClass;
typedef std::shared_ptr<OtBloomClass> OtBloom;

class OtBloomClass : public OtFilterClass {
public:
	// constructor/destructor
	OtBloomClass();
	~OtBloomClass();

	// execute filter
	void execute(int view, int w, int h) override;

private:
	// BGFX shader
	bgfx::UniformHandle bloomUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};
