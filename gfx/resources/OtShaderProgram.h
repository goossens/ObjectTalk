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
#include <string>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "OtBgfxHandle.h"


//
//	Rendering state synonyms (to keep BGFX out of other modules)
//

static constexpr uint64_t OtStateWriteRgb = BGFX_STATE_WRITE_RGB;
static constexpr uint64_t OtStateWriteA = BGFX_STATE_WRITE_A;
static constexpr uint64_t OtStateWriteZ = BGFX_STATE_WRITE_Z;

static constexpr uint64_t OtStateDepthTestLess = BGFX_STATE_DEPTH_TEST_LESS;
static constexpr uint64_t OtStateDepthTestLessEqual = BGFX_STATE_DEPTH_TEST_LEQUAL;
static constexpr uint64_t OtStateDepthTestGreater = BGFX_STATE_DEPTH_TEST_GREATER;
static constexpr uint64_t OtStateDepthTestGreaterEqual = BGFX_STATE_DEPTH_TEST_GEQUAL;

static constexpr uint64_t OtStateCullCw = BGFX_STATE_CULL_CW;
static constexpr uint64_t OtStateCullCcw = BGFX_STATE_CULL_CCW;

static constexpr uint64_t OtStateLines = BGFX_STATE_PT_LINES;

static constexpr uint64_t OtStateBlendAlpha = BGFX_STATE_BLEND_ALPHA;
static constexpr uint64_t OtStateBlendAdd = BGFX_STATE_BLEND_ADD;

static constexpr uint64_t OtStateMsaa = BGFX_STATE_MSAA;


//
//	OtShaderProgram
//

class OtShaderProgram {
public:
	// constructors/destructor
	OtShaderProgram() = default;
	OtShaderProgram(const char* vertex, const char* fragment);

	// initialize program
	void initialize(const char* vertex, const char* fragment);

	// clear the resources
	inline void clear() { program.clear(); }

	// see if program is valid
	inline bool isValid() { return program.isValid(); }

	// set parameters for the next run
	inline void setState(uint64_t state) { bgfx::setState(state); }
	inline void setTransform(const glm::mat4& transform) { bgfx::setTransform(glm::value_ptr(transform)); }
	inline void setTransforms(const glm::mat4* transforms, size_t count) { bgfx::setTransform(transforms, static_cast<uint16_t>(count)); }
	inline void setVertexCount(size_t count) { bgfx::setVertexCount(static_cast<uint32_t>(count)); }
	inline void setInstanceCount(size_t count) { bgfx::setInstanceCount(static_cast<uint32_t>(count)); }

	// run program in the specified view on the GPU
	void submit(bgfx::ViewId view);

private:
	// shader names
	std::string vertexShaderName;
	std::string fragmentShaderName;

	// GPU program
	OtBgfxHandle<bgfx::ProgramHandle> program;
};
