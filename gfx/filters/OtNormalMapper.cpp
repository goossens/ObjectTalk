//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

void OtNormalMapper::execute(OtPass& pass, int w, int h) {
	// convert heightmap to normalmap
	uniform.setValue(0, glm::vec4(1.0 / float(w), 1.0 / float(h), normalStrength, float(includeHeightFlag)));
	uniform.submit();
	pass.runShaderProgram(program);
}