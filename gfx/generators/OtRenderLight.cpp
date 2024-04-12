//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRenderLight.h"


//
//	OtRenderLight::execute
//

void OtRenderLight::execute(OtPass& pass) {
	uniform.setValue(0, center, size);
	uniform.setValue(1, color, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
