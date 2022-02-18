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


// vertex definition
struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;

	static bgfx::VertexLayout getLayout() {
		static bool ready = false;
		static bgfx::VertexLayout layout;

		if (!ready) {
			layout.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();

			ready = true;
		}

		return layout;
	}
};


//
//	OtFilterClass::OtFilterClass
//

OtFilterClass::OtFilterClass() {
	textureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);
}


//
//	OtFilterClass::~OtFilterClass
//

OtFilterClass::~OtFilterClass() {
	bgfx::destroy(textureUniform);
}


//
//	OtFilterClass::createQuad
//

void OtFilterClass::createQuad(int w, int h) {
	if (bgfx::getAvailTransientVertexBuffer(3, Vertex::getLayout()) == 3) {
		const bgfx::Caps* caps = bgfx::getCaps();

		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, Vertex::getLayout());
		Vertex* vertex = (Vertex*) vb.data;

		vertex[0].position = glm::vec3(-w, 0.0, 0.0);
		vertex[0].uv = caps->originBottomLeft ? glm::vec2(-1.0, 1.0) : glm::vec2(-1.0, 0.0);

		vertex[1].position = glm::vec3(w, 0.0, 0.0);
		vertex[1].uv = caps->originBottomLeft ? glm::vec2(1.0, 1.0) : glm::vec2(1.0, 0.0);

		vertex[2].position = glm::vec3(w, h * 2.0, 0.0);
		vertex[2].uv = caps->originBottomLeft ? glm::vec2(1.0, -1.0) : glm::vec2(1.0, 2.0);

		bgfx::setVertexBuffer(0, &vb);
	}
}


//
//	OtFilterClass::render
//

void OtFilterClass::render(int w, int h, bgfx::TextureHandle texture, bgfx::FrameBufferHandle fb) {
	// get next view ID
	bgfx::ViewId view = OtFrameworkClass::instance()->getNextViewID();

	// create "single triangle quad" to cover area
	createQuad(w, h);

	// setup BGFX context
	bgfx::setTexture(0, textureUniform, texture);
	bgfx::setViewRect(view, 0, 0, w, h);
	bgfx::setViewFrameBuffer(view, fb);
	bgfx::setState(state);

	glm::mat4 matrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(matrix));

	// execure filter
	execute(view, w, h);
}


//
//	OtFilterClass::render
//

void OtFilterClass::render(int x, int y, int w, int h, bgfx::TextureHandle texture) {
	// get next view ID
	bgfx::ViewId view = OtFrameworkClass::instance()->getNextViewID();

	// create "single triangle quad" to cover area
	createQuad(w, h);

	// setup BGFX context
	bgfx::setTexture(0, textureUniform, texture);
	bgfx::setViewRect(view, x, y, w, h);
	bgfx::setState(state);

	glm::mat4 matrix = glm::ortho(0.0f, (float) w, (float) h, 0.0f, -1.0f, 1.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(matrix));

	// execure filter
	execute(view, w, h);
}
