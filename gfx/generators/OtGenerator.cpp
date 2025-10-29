//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

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
	static constexpr float threadCount = 8.0f;

	pass.runComputeProgram(
		preparePass(),
		static_cast<int>(std::ceil(texture.getWidth() / threadCount)),
		static_cast<int>(std::ceil(texture.getHeight() / threadCount)),
		1);
}
