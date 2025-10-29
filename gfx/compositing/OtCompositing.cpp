//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCompositing.h"
#include "OtPass.h"


//
//	OtCompositing::render
//

void OtCompositing::render(OtTexture& origin, OtFrameBuffer& destination) {
	// setup a compositing pass
	OtPass pass;
	pass.setFrameBuffer(destination);
	pass.setState(getState());
	pass.submitQuad(destination.getWidth(), destination.getHeight());

	uniform.setValue(0, brightness, 0.0f, 0.0f, 0.0f);
	uniform.submit();

	sampler.submit(0, origin);

	// execute compositing filter
	pass.runShaderProgram(program);
}

void OtCompositing::render(OtFrameBuffer& origin, OtFrameBuffer& destination) {
	auto texture = origin.getColorTexture();
	render(texture, destination);
}
