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

#include "OtCubeMap.h"
#include "OtTexture.h"


//
//	OtSampler
//

class OtSampler {
public:
	// constructor/destructor
	OtSampler() = default;
	OtSampler(const char* name);
	~OtSampler();

	// initialize sampler
	void initialize(const char* name);

	// clear the resources
	void clear();

	// bind texture to sampler and submit to GPU
	void submit(int unit);
	void submit(int unit, OtTexture& texture);
	void submit(int unit, bgfx::TextureHandle texture);
	void submit(int unit, OtCubeMap& cubemap);

private:
	// uniform
	bgfx::UniformHandle uniform = BGFX_INVALID_HANDLE;
};
