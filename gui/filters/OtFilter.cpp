//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtFilter.h"
#include "OtFramework.h"
#include "OtQuad.h"


//
//	OtFilterClass::OtFilterClass
//

OtFilterClass::OtFilterClass() {
	textureUniform = OtFrameworkClass::instance()->getUniform("s_texture", bgfx::UniformType::Sampler);
}


//
//	OtFilterClass::render
//

void OtFilterClass::render(int w, int h, bgfx::TextureHandle texture, bgfx::FrameBufferHandle fb) {
	// get next view ID
	bgfx::ViewId view = OtFrameworkClass::instance()->getNextViewID();

	// create "single triangle quad" to cover area
	OtQuadSubmit(w, h);

	// setup BGFX context
	bgfx::setTexture(0, textureUniform, texture);
	bgfx::setViewRect(view, 0, 0, w, h);
	bgfx::setViewFrameBuffer(view, fb);
	bgfx::setState(state);

	glm::mat4 matrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(matrix));

	// execute filter
	execute(view, w, h);
}


//
//	OtFilterClass::render
//

void OtFilterClass::render(int x, int y, int w, int h, bgfx::TextureHandle texture) {
	// get next view ID
	bgfx::ViewId view = OtFrameworkClass::instance()->getNextViewID();

	// create "single triangle quad" to cover area
	OtQuadSubmit(w, h);

	// setup BGFX context
	bgfx::setTexture(0, textureUniform, texture);
	bgfx::setViewRect(view, x, y, w, h);
	bgfx::setState(state);

	glm::mat4 matrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(matrix));

	// execure filter
	execute(view, w, h);
}
