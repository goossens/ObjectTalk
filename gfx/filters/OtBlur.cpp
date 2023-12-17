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
	uniform.setValue(
		0,
		horizontalScale == -1.0f ? (0.2f / float(w)) : horizontalScale,
		verticalScale == -1.0f ? (0.2f / float(h)) : verticalScale,
		intensity == -1.0f ? 2.0f : intensity,
		alpha == -1.0f ? 1.0f : alpha);

	uniform.submit();
	pass.runShaderProgram(program);
}
