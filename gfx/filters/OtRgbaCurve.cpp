//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtRgbaCurve.h"


//
//	OtRgbaCurve::execute
//

void OtRgbaCurve::execute(OtPass& pass) {
	uniform.setValue(0, float(curve), 0.0f, 0.0f, 0.0f);
	uniform.setValue(1, blackLevel);
	uniform.setValue(2, whiteLevel);
	uniform.submit();

	if (!lutTexture.isValid()) {

	}

	lutSampler.submit(1, lutTexture);
	pass.runShaderProgram(program);
}
