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
#include "OtSampler.h"
#include "OtPass.h"


//
//	OtGenerator
//

class OtGenerator {
public:
	// destructor
	virtual ~OtGenerator() {}

	// set rendering state
	void setState(int s) { state = s; }

	// run generator
	void render(OtFrameBuffer& destination);
	void render(int w, int h);

private:
	// execute generator
	virtual void execute(OtPass& pass) {}

	// rendering state
	uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
};
