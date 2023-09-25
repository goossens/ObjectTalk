//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtGenerator.h"
#include "OtPass.h"


//
//	OtGenerator::render
//

void OtGenerator::render(int w, int h, OtFrameBuffer& destination) {
	// setup generator pass
	OtPass pass;
	pass.setFrameBuffer(destination);
	pass.submitQuad(w, h);

	// execute generator
	bgfx::setState(state);
	execute(pass, w, h);
}


//
//	OtGenerator::render
//

void OtGenerator::render(int w, int h) {
	// setup generator pass
	OtPass pass;
	pass.submitQuad(w, h);

	// execute generator
	bgfx::setState(state);
	execute(pass, w, h);
}
