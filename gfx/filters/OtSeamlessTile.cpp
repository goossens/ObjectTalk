//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtSeamlessTile.h"


//
//	OtSeamlessTile::execute
//

void OtSeamlessTile::execute(OtPass& pass) {
	// turn image into a seamless tile
	pass.runShaderProgram(program);
}