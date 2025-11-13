//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFilter2.h"


//
//	OtFilter2::render
//

void OtFilter2::render(OtTexture& origin, OtFrameBuffer& destination) {
	// setup filtering pass
	OtPass pass;
	auto texture = destination.getColorTexture();
	pass.setImage(0, origin, 0, OtPass::readAccess);
	pass.setImage(1, texture, 0, OtPass::writeAccess);

	// execute generator
	static constexpr float threadCount = 8.0f;

	pass.runComputeProgram(
		preparePass(),
		static_cast<int>(std::ceil(texture.getWidth() / threadCount)),
		static_cast<int>(std::ceil(texture.getHeight() / threadCount)),
		1);
}

void OtFilter2::render(OtFrameBuffer& origin, OtFrameBuffer& destination) {
	auto texture = origin.getColorTexture();
	render(texture, destination);
}
