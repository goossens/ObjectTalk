//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtGenerator.h"
#include "OtPass.h"


//
//	OtGenerator::render
//

void OtGenerator::render(OtFrameBuffer& destination) {
	// setup generator pass
	OtPass pass;
	pass.setFrameBuffer(destination);
	pass.submitQuad(destination.getWidth(), destination.getHeight());

	// execute generator
	bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
	execute(pass);
}
