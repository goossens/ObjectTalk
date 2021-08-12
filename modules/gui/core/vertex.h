//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "bgfx/bgfx.h"

#include "vertex.h"


//
//	OtVertex
//

struct OtVertex {
	// elements of a vertex
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec4 color;

	// constructors
	OtVertex() {}
	OtVertex(glm::vec3 p, glm::vec3 n, glm::vec2 u, glm::vec4 c = glm::vec4(1.0))
		: position(p), normal(n), uv(u), color(c) {}

	// get a BGFX vertex description
	static bgfx::VertexLayout getVertexLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};
