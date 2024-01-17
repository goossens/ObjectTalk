//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtFilter.h"
#include "OtPass.h"


//
//	OtFilter::render
//

void OtFilter::render(OtTexture& origin, OtFrameBuffer& destination) {
	// setup a filtering pass
	OtPass pass;
	pass.setFrameBuffer(destination);
	pass.submitQuad(origin.getWidth(), origin.getHeight());

	// execute filter
	textureSampler.submit(0, origin);
	bgfx::setState(state);
	execute(pass);
}

void OtFilter::render(OtTexture& origin) {
	// setup a filtering pass
	OtPass pass;
	pass.submitQuad(origin.getWidth(), origin.getHeight());

	// execute filter
	textureSampler.submit(0, origin);
	bgfx::setState(state);
	execute(pass);
}

void OtFilter::render(OtFrameBuffer& origin, OtFrameBuffer& destination) {
	auto texture = origin.getColorTexture();
	render(texture, destination);
}

void OtFilter::render(OtFrameBuffer& origin) {
	auto texture = origin.getColorTexture();
	render(texture);
}
