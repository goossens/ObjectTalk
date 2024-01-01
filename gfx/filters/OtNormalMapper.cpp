//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtNormalMapper.h"


//
//	OtNormalMapper::execute
//

void OtNormalMapper::execute(OtPass& pass) {
	// convert heightmap to normalmap
	uniform.setValue(0, normalStrength, float(includeHeightFlag), 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}