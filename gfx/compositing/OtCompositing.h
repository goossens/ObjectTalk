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
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"
#include "OtUniformVec4.h"


//
//	OtCompositing
//

class OtCompositing {
public:
	// destructor
	virtual inline ~OtCompositing() {}

	// set properties
	inline void setBrightness(float value) { brightness = value; }

	// render filter
	void render(OtTexture& origin, OtFrameBuffer& destination);
	void render(OtFrameBuffer& origin, OtFrameBuffer& destination);

private:
	// get render state
	virtual uint64_t getState() = 0;

	// GPU resourcess
	float brightness = 1.0f;

	OtSampler sampler{"s_texture", OtSampler::pointSampling | OtSampler::clampSampling};
	OtUniformVec4 uniform = OtUniformVec4("u_compositing", 1);
	OtShaderProgram program = OtShaderProgram("OtCompositingVS", "OtCompositingFS");
};
