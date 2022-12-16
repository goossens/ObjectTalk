//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	OtShader
//

class OtShader {
public:
	// constructors/destructor
	OtShader() = default;
	OtShader(const char* vertex, const char* fragment);
	~OtShader();

	// initialize uniform
	void initialize(const char* vertex, const char* fragment);

	// clear the resources
	void clear();

	// run shader for specified view on GPU
	void submit(bgfx::ViewId view);

private:
	// uniform
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};
