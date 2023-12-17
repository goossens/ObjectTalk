//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtTileableFbm.h"


//
//	OtTileableFbm::execute
//

void OtTileableFbm::execute(OtPass& pass, int w, int h) {
	uniform.setValue(0, frequency, lacunarity, amplitude, persistence);
	uniform.setValue(1, octaves, 0.0f, 0.0f, 0.0f);
	uniform.submit();
	pass.runShaderProgram(program);
}
