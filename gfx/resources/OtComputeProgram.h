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
#include <string>

#include "OtBgfxHandle.h"


//
//	OtComputeProgram
//

class OtComputeProgram {
public:
	// constructors/destructor
	OtComputeProgram() = default;
	OtComputeProgram(const char* compute);

	// initialize program
	void initialize(const char* compute);

	// clear the resources
	inline void clear() { program.clear(); }

	// see if program is valid
	inline bool isValid() { return program.isValid(); }

	// run program in the specified view on the GPU
	void dispatch(bgfx::ViewId view, uint32_t x, uint32_t y, uint32_t z);

private:
	// shader name
	std::string computeShaderName;

	// GPU program
	OtBgfxHandle<bgfx::ProgramHandle> program;
};
