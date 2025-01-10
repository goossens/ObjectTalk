//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLinearizeDepth.h"


//
//	OtLinearizeDepth::execute
//

void OtLinearizeDepth::execute(OtPass& pass) {
	uniform.setValue(0, nearPlane, farPlane, 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
