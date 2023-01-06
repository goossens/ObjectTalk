//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "OtGpu.h"
#include "OtVertex.h"


//
//	Functions
//

inline void OtQuadSubmit(int w, int h) {
	// submit a "single triangle" as a quad covering a complete rectangle
	if (bgfx::getAvailTransientVertexBuffer(3, OtVertexPosTex::getLayout()) == 3) {
		bool originBottomLeft = OtGpuHasOriginBottomLeft();

		bgfx::TransientVertexBuffer vb;
		bgfx::allocTransientVertexBuffer(&vb, 3, OtVertexPosTex::getLayout());
		OtVertexPosTex* vertex = (OtVertexPosTex*) vb.data;

		vertex[0].position = glm::vec3(-w, 0.0, 0.0);
		vertex[0].uv = originBottomLeft ? glm::vec2(-1.0, 1.0) : glm::vec2(-1.0, 0.0);

		vertex[1].position = glm::vec3(w, 0.0, 0.0);
		vertex[1].uv = originBottomLeft ? glm::vec2(1.0, 1.0) : glm::vec2(1.0, 0.0);

		vertex[2].position = glm::vec3(w, h * 2.0, 0.0);
		vertex[2].uv = originBottomLeft ? glm::vec2(1.0, -1.0) : glm::vec2(1.0, 2.0);

		bgfx::setVertexBuffer(0, &vb);
	}
}
