//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPixelate.h"


//
//	OtPixelate::execute
//

void OtPixelate::execute(OtPass& pass) {
	uniform.setValue(0, static_cast<float>(size), 0.0f, 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
