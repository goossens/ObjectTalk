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
#include "glm/glm.hpp"


//
//	OtTerrainVertex
//

struct OtTerrainVertex {
	glm::vec3 position;
	glm::vec3 normal;

	// constructors
	OtTerrainVertex() = default;
	OtTerrainVertex(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {}

	// get a BGFX vertex description
	static bgfx::VertexLayout getVertexLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};
