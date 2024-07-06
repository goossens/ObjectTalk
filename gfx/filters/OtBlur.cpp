//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtBlur.h"


//
//	OtBlur::execute
//

void OtBlur::execute(OtPass& pass) {
	uniform.setValue(
		0,
		intensity == -1.0f ? 2.0f : intensity,
		alpha == -1.0f ? 1.0f : alpha,
		horizontalScale,
		verticalScale);

	uniform.submit();
	pass.runShaderProgram(program);
}
