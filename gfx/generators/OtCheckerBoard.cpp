//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCheckerBoard.h"


//
//	OtCheckerBoard::execute
//

void OtCheckerBoard::execute(OtPass& pass) {
	uniform.setValue(0, repeat, 0.0f, 0.0f, 0.0f);
	uniform.setValue(1, blackColor);
	uniform.setValue(2, whiteColor);
	uniform.submit();
	pass.runShaderProgram(program);
}
