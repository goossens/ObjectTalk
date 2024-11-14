//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtContrastSaturationBrightness.h"


//
//	OtContrastSaturationBrightness::execute
//

void OtContrastSaturationBrightness::execute(OtPass& pass) {
	uniform.setValue(0, contrast, saturation, brightness, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
