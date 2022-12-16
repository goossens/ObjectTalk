//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "bgfx/bgfx.h"

#include "OtFrameBuffer.h"
#include "OtTexture.h"
#include "OtSampler.h"
#include "OtPass.h"


//
//	OtFilterClass
//

class OtFilterClass;
typedef std::shared_ptr<OtFilterClass> OtFilter;

class OtFilterClass {
public:
	// destructor
	virtual ~OtFilterClass() {}

	// set rendering state
	void setState(int s) { state = s; }

	// execute filter
	virtual void execute(OtPass& pass, int w, int h) {}

	// render filter
	void render(int w, int h, OtFrameBuffer& origin, OtFrameBuffer& destination);
	void render(int x, int y, int w, int h, OtFrameBuffer& origin);

private:
	// the texture sampler
	OtSampler textureSampler = OtSampler("s_texture");

	// rendering state
	uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A;
};
