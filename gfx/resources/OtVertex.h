//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtVertex.h"


//
//	OtVertex
//

struct OtVertex {
	// vertex elements
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 uv;

	// constructors
	OtVertex() = default;

	inline OtVertex(const glm::vec3& p, const glm::vec3& n=glm::vec3(0.0f), const glm::vec2& u=glm::vec2(0.0f), const glm::vec3 t=glm::vec3(0.0f), const glm::vec3 b=glm::vec3(0.0f)) :
		position(p), normal(n), tangent(t), bitangent(b), uv(u) {
	}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};


//
//	OtVertexPosUvCol2D
//

struct OtVertexPosCol2D {
	// vertex elements
	glm::vec2 position;
	uint32_t color;

	// constructors
	OtVertexPosCol2D() = default;
	inline OtVertexPosCol2D(const glm::vec2& p, uint32_t c=0) : position(p), color(c) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();

		return layout;
	}
};


//
//	OtVertexPosUvCol2D
//

struct OtVertexPosUvCol2D {
	// vertex elements
	glm::vec2 position;
	glm::vec2 uv;
	uint32_t color;

	// constructors
	OtVertexPosUvCol2D() = default;
	inline OtVertexPosUvCol2D(const glm::vec2& p, const glm::vec2& u=glm::vec2(0.0f), uint32_t c=0) : position(p), uv(u), color(c) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();

		return layout;
	}
};


//
//	OtVertexPosUv
//

struct OtVertexPosUv {
	// vertex elements
	glm::vec3 position;
	glm::vec2 uv;

	// constructors
	OtVertexPosUv() = default;
	inline OtVertexPosUv(const glm::vec3& p, const glm::vec2& u=glm::vec2(0.0f)) : position(p), uv(u) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};


//
//	OtVertexPosUv2D
//

struct OtVertexPosUv2D {
	// vertex elements
	glm::vec2 position;
	glm::vec2 uv;

	// constructors
	OtVertexPosUv2D() = default;
	inline OtVertexPosUv2D(const glm::vec2& p, const glm::vec2& u=glm::vec2(0.0f)) : position(p), uv(u) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};


//
//	OtVertexPosUvw
//

struct OtVertexPosUvw {
	// vertex elements
	glm::vec3 position;
	glm::vec3 uvw;

	// constructors
	OtVertexPosUvw() = default;
	inline OtVertexPosUvw(const glm::vec3& p, const glm::vec3& u=glm::vec3(0.0f)) : position(p), uvw(u) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 3, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};


//
//	OtVertexPos
//

struct OtVertexPos {
	// vertex elements
	glm::vec3 position;

	// constructors
	OtVertexPos() = default;
	inline OtVertexPos(const glm::vec3& p) : position(p) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};


//
//	OtVertexBones
//

struct OtVertexBones {
	// vertex elements
	glm::vec4 indices;
	glm::vec4 weights;

	// constructors
	OtVertexBones() = default;
	inline OtVertexBones(const glm::vec4& i, const glm::vec4& w) : indices(i), weights(w) {}

	// get vertex description
	static inline bgfx::VertexLayout getLayout() {
		bgfx::VertexLayout layout;

		layout.begin()
			.add(bgfx::Attrib::Indices, 4, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Weight, 4, bgfx::AttribType::Float)
			.end();

		return layout;
	}
};
