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
//	OtBlitClass::execute
//

void OtBlitClass::execute(OtPass& pass, int w, int h) {
	uniform.set(0, glm::vec4(intensity == -1.0 ? 1.0 : intensity, alpha == -1.0 ? 1.0 : alpha, 0.0, 0.0));
	uniform.submit();
	pass.runShader(shader);
}
