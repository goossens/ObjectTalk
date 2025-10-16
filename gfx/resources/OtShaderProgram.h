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

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "OtBgfxHandle.h"


//
//	OtShaderProgram
//

class OtShaderProgram {
public:
	// constructors/destructor
	OtShaderProgram() = default;
	OtShaderProgram(const char* vertex, const char* fragment);

	// initialize program
	void initialize(const char* vertex, const char* fragment);

	// clear the resources
	inline void clear() { program.clear(); }

	// see if program is valid
	inline bool isValid() { return program.isValid(); }

	// run program in the specified view on the GPU
	void dispatch(bgfx::ViewId view, uint8_t discard);

private:
	// shader names
	std::string vertexShaderName;
	std::string fragmentShaderName;

	// GPU program
	OtBgfxHandle<bgfx::ProgramHandle> program;
};
