//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "vertex.h"


//
//	OtVertex
//

struct OtVertex {
	// elements of a vertex
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;

	// constructors
	OtVertex() = default;
	OtVertex(glm::vec3 p, glm::vec3 n, glm::vec2 u) : position(p), normal(n), uv(u) {}

	// get a BGFX vertex description
	static bgfx::VertexLayout getVertexLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};
