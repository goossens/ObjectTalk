//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtBlur.h"


//
//	OtBlur::OtBlur
//

OtBlur::OtBlur() {
	setFlags(OtSampler::clampSampling);
}


//
//	OtBlur::execute
//

void OtBlur::execute(OtPass& pass) {
	uniform.setValue(0, intensity, alpha, direction);
	uniform.submit();
	pass.runShaderProgram(program);
}
