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
	uniform.set(0, glm::vec4(0.2 / (float) w, 0.2 / (float) h, 2.0, 1.0));
	uniform.submit();
	pass.runShader(shader);
}
