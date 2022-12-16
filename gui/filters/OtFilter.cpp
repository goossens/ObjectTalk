//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtFilter.h"
#include "OtQuad.h"
#include "OtPass.h"


//
//	OtFilterClass::render
//

void OtFilterClass::render(int w, int h, OtFrameBuffer& origin, OtFrameBuffer& destination) {
	// setup a filtering pass
	OtPass pass;
	pass.reserveRenderingSlot();
	pass.setRectangle(0, 0, w, h);
	pass.setFrameBuffer(destination);

	glm::mat4 matrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
	pass.setTransform(glm::mat4(1.0), matrix);

	// create single "triangular quad" to cover area
	OtQuadSubmit(w, h);
	bgfx::setState(state);
	origin.bindColorTexture(textureSampler, 0);

	// execute filter
	execute(pass, w, h);
}


//
//	OtFilterClass::render
//

void OtFilterClass::render(int x, int y, int w, int h, OtFrameBuffer& origin) {
	// setup a filtering pass
	OtPass pass;
	pass.reserveRenderingSlot();
	pass.setRectangle(x, y, w, h);

	glm::mat4 matrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
	pass.setTransform(glm::mat4(1.0), matrix);

	// create "single triangle quad" to cover area
	OtQuadSubmit(w, h);
	bgfx::setState(state);
	origin.bindColorTexture(textureSampler, 0);

	// execure filter
	execute(pass, w, h);
}
