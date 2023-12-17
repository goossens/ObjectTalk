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

#include "OtFilter.h"
#include "OtPass.h"


//
//	OtFilter::render
//

void OtFilter::render(OtFrameBuffer& origin, OtFrameBuffer& destination) {
	// setup a filtering pass
	OtPass pass;
	pass.setFrameBuffer(destination);
	pass.submitQuad(origin.getWidth(), origin.getHeight());

	// execute filter
	origin.bindColorTexture(textureSampler, 0);
	bgfx::setState(state);
	execute(pass);
}


//
//	OtFilter::render
//

void OtFilter::render(OtFrameBuffer& origin) {
	// setup a filtering pass
	OtPass pass;
	pass.submitQuad(origin.getWidth(), origin.getHeight());

	// execute filter
	origin.bindColorTexture(textureSampler, 0);
	bgfx::setState(state);
	execute(pass);
}
