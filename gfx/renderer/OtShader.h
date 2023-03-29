//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtBgfxHandle.h"


//
//	Rendering state synonymns (to keep BGFX out of other modules)
//

static constexpr uint64_t OtStateWriteRgb = BGFX_STATE_WRITE_RGB;
static constexpr uint64_t OtStateWriteA = BGFX_STATE_WRITE_A;
static constexpr uint64_t OtStateWriteZ = BGFX_STATE_WRITE_Z;

static constexpr uint64_t OtStateDepthTestLess = BGFX_STATE_DEPTH_TEST_LESS;
static constexpr uint64_t OtStateDepthTestAlways = BGFX_STATE_DEPTH_TEST_ALWAYS;

static constexpr uint64_t OtStateCullCw = BGFX_STATE_CULL_CW;
static constexpr uint64_t OtStateCullCcw = BGFX_STATE_CULL_CCW;

static constexpr uint64_t OtStateLines = BGFX_STATE_PT_LINES;

static constexpr uint64_t OtStateBlendAlpha = BGFX_STATE_BLEND_ALPHA;
static constexpr uint64_t OtStateBlendAdd = BGFX_STATE_BLEND_ADD;

static constexpr uint64_t OtStateMsaa = BGFX_STATE_MSAA;


//
//	OtShader
//

class OtShader {
public:
	// constructors/destructor
	OtShader() = default;
	OtShader(const char* vertex, const char* fragment);

	// initialize uniform
	void initialize(const char* vertex, const char* fragment);

	// clear the resources
	void clear() { shader.clear(); }

	// see if shader is valid
	bool isValid() { return shader.isValid(); }

	// set parameters for the next run
	void setState(uint64_t state) { bgfx::setState(state); }
	void setTransform(const glm::mat4& transform) { bgfx::setTransform(glm::value_ptr(transform)); }

	// run shader in the specified view on the GPU
	void submit(bgfx::ViewId view);

private:
	// shader program
	std::string vertexShaderName;
	std::string fragmentShaderName;
	OtBgfxHandle<bgfx::ProgramHandle> shader;
};