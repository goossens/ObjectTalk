//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtBlit.h"


//
//	OtBlit::execute
//

void OtBlit::execute(OtPass& pass) {
	uniform.setValue(0, intensity, alpha, 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
