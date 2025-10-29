//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtGaussian.h"


//
//	OtGaussian::OtGaussian
//

OtGaussian::OtGaussian() {
	flags = OtSampler::linearSampling | OtSampler::clampSampling;
}


//
//	OtGaussian::execute
//

void OtGaussian::execute(OtPass& pass) {
	uniform.setValue(0, direction * radius, 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
