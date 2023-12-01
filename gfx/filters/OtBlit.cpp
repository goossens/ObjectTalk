//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtBlit.h"


//
//	OtBlit::execute
//

void OtBlit::execute(OtPass& pass, int w, int h) {
	uniform.setValue(0, glm::vec4(intensity == -1.0 ? 1.0 : intensity, alpha == -1.0 ? 1.0 : alpha, 0.0, 0.0));
	uniform.submit();
	pass.runShaderProgram(program);
}
