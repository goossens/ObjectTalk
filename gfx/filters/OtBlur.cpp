//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtBlur.h"


//
//	OtBlur::execute
//

void OtBlur::execute(OtPass& pass, int w, int h) {
	uniform.set(0, glm::vec4(
		horizontalScale == -1.0 ? (0.2 / (float) w) : horizontalScale,
		verticalScale == -1.0 ? (0.2 / (float) h) : verticalScale,
		intensity == -1.0 ? 2.0 : intensity,
		alpha == -1.0 ? 1.0 : alpha));

	uniform.submit();
	pass.runShaderProgram(program);
}
