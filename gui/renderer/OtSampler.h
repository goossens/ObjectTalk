//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtBgfxHandle.h"

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

	// initialize sampler
	void initialize(const char* name);

	// clear the resources
	void clear() { uniformName.clear(); uniform.clear(); }

	// see if sampler is valid
	bool isValid() { return uniform.isValid(); }

	// bind texture to sampler and submit to GPU
	void submit(int unit, const char* name=nullptr); // bind dummy texture
	void submit(int unit, OtTexture& texture, const char* name=nullptr);
	void submit(int unit, bgfx::TextureHandle texture, const char* name=nullptr);
	void submit(int unit, OtCubeMap& cubemap, const char* name=nullptr);

private:
	// uniform
	std::string uniformName;
	OtBgfxHandle<bgfx::UniformHandle> uniform;
};
