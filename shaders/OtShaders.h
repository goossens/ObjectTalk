//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "bgfx/bgfx.h"


//
//	OtShaders
//

class OtShaders {
public:
	// get a named shader
	static bgfx::ShaderHandle get(const char* name);
	inline static bgfx::ShaderHandle get(const std::string& name) { return get(name.c_str()); }
};
