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

#include "OtFrameBuffer.h"
#include "OtPass.h"
#include "OtSampler.h"
#include "OtTexture.h"


//
//	OtFilter
//

class OtFilter {
public:
	// destructor
	virtual inline ~OtFilter() {}

	// set rendering state
	inline void setFlags(uint64_t f) { flags = f; }
	inline void setState(uint64_t s) { state = s; }

	// render filter
	void render(OtTexture& origin, OtFrameBuffer& destination);
	void render(OtFrameBuffer& origin, OtFrameBuffer& destination);

private:
	// execute filter
	virtual void execute(OtPass& pass) = 0;

	// the texture sampler
	OtSampler textureSampler{"s_texture"};

	// rendering state
	uint64_t flags = OtSampler::pointSampling | OtSampler::clampSampling;
	uint64_t state = OtStateWriteRgb | OtStateWriteZ;
};
