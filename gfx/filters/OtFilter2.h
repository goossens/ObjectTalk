//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "OtComputeProgram.h"
#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtTexture.h"


//
//	OtFilter2
//

class OtFilter2 {
public:
	// destructor
	virtual inline ~OtFilter2() {}

	// set rendering state
	inline void setFlags(uint64_t f) { flags = f; }

	// render filter
	void render(OtTexture& origin, OtFrameBuffer& destination);
	void render(OtFrameBuffer& origin, OtFrameBuffer& destination);

private:
	// prepare generator pass
	virtual OtComputeProgram& preparePass() = 0;

	// the texture sampler
	OtSampler textureSampler{"s_texture"};

	// rendering state
	uint64_t flags = OtSampler::pointSampling | OtSampler::clampSampling;
};
