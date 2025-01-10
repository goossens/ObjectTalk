//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtComputeProgram.h"
#include "OtShaders.h"


//
//	OtComputeProgram::OtComputeProgram
//

OtComputeProgram::OtComputeProgram(const char* compute) {
	initialize(compute);
}


//
//	OtComputeProgram::initialize
//

void OtComputeProgram::initialize(const char* compute) {
	computeShaderName = compute;
}


//
//	OtComputeProgram::dispatch
//

void OtComputeProgram::dispatch(bgfx::ViewId view, uint32_t x, uint32_t y, uint32_t z) {
	if (!isValid()) {
		if (computeShaderName.size()) {
			auto computeShader = OtShaders::get(computeShaderName);
			program = bgfx::createProgram(computeShader, true);

			if (!isValid()) {
				OtLogFatal("Internal error: can't create compute program");
			}

		} else {
			OtLogFatal("Internal error: compute program not initialized before submission");
		}
	}

	bgfx::dispatch(view, program.getHandle(), x, y, z);
}
