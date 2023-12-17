//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtBloom.h"


//
//	OtBloom::execute
//

void OtBloom::execute(OtPass& pass, int w, int h) {
	uniform.setValue(0, 0.2f / float(w), 0.2f / float(h), 2.0f, 1.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
