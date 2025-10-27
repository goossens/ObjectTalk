//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtGenerator.h"
#include "OtPass.h"


//
//	OtGenerator::render
//

void OtGenerator::render(OtFrameBuffer& destination) {
	// setup generator pass
	OtPass pass;
	auto texture = destination.getColorTexture();
	pass.setImage(0, texture, 0, OtPass::writeAccess);

	// execute generator
	pass.runComputeProgram(
		preparePass(),
		texture.getWidth() / threadCount,
		texture.getHeight() / threadCount,
		1);
}
