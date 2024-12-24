//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtIslandizer.h"


//
//	OtIslandizer::execute
//

void OtIslandizer::execute(OtPass& pass) {
	// Run islander filter
	uniform.setValue(0, static_cast<float>(distanceFunction), 0.0f, 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}