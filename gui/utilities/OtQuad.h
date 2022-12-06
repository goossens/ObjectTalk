//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"


//
//	Vertex definition
//

// vertex definition
struct OtQuadVertex {
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
//	Functions
//

inline void OtQuadSubmit(int w, int h) {
	// submit a "single triangle" as a quad covering a complete rectangle
	if (bgfx::getAvailTransientVertexBuffer(3, OtQuadVertex::getLayout()) == 3) {
		const bgfx::Caps* caps = bgfx::getCaps();

		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, OtQuadVertex::getLayout());
		OtQuadVertex* vertex = (OtQuadVertex*) vb.data;

		vertex[0].position = glm::vec3(-w, 0.0, 0.0);
		vertex[0].uv = caps->originBottomLeft ? glm::vec2(-1.0, 1.0) : glm::vec2(-1.0, 0.0);

		vertex[1].position = glm::vec3(w, 0.0, 0.0);
		vertex[1].uv = caps->originBottomLeft ? glm::vec2(1.0, 1.0) : glm::vec2(1.0, 0.0);

		vertex[2].position = glm::vec3(w, h * 2.0, 0.0);
		vertex[2].uv = caps->originBottomLeft ? glm::vec2(1.0, -1.0) : glm::vec2(1.0, 2.0);

		bgfx::setVertexBuffer(0, &vb);
	}
}
