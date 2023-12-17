//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtFrameBuffer.h"
#include "OtTexture.h"
#include "OtSampler.h"
#include "OtPass.h"


//
//	OtFilter
//

class OtFilter {
public:
	// destructor
	virtual ~OtFilter() {}

	// set rendering state
	inline void setState(int s) { state = s; }

	// render filter
	void render(OtFrameBuffer& origin, OtFrameBuffer& destination);
	void render(OtFrameBuffer& origin);

private:
	// execute filter
	virtual void execute(OtPass& pass) {}

	// the texture sampler
	OtSampler textureSampler{"s_texture", OtSampler::pointSampling | OtSampler::clampSampling};

	// rendering state
	uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
};
