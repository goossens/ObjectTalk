//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFilter.h"


//
//	OtFilter::render
//

void OtFilter::render(OtTexture& origin, OtFrameBuffer& destination) {
	// setup a filtering pass
	OtPass pass;
	pass.setFrameBuffer(destination);
	pass.submitQuad(destination.getWidth(), destination.getHeight());
	pass.setState(state);

	// execute filter
	textureSampler.setFlags(flags);
	textureSampler.submit(0, origin);
	execute(pass);
}

void OtFilter::render(OtFrameBuffer& origin, OtFrameBuffer& destination) {
	auto texture = origin.getColorTexture();
	render(texture, destination);
}
