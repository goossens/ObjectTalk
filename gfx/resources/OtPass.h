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


#include "OtComputeProgram.h"
#include "OtCubeMap.h"
#include "OtFrameBuffer.h"
#include "OtGbuffer.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"


//
//	OtPass
//

class OtPass {
public:
	// pipeline states
	static constexpr uint64_t stateWriteRgb = BGFX_STATE_WRITE_RGB;
	static constexpr uint64_t stateWriteA = BGFX_STATE_WRITE_A;
	static constexpr uint64_t stateWriteZ = BGFX_STATE_WRITE_Z;

	static constexpr uint64_t stateDepthTestLess = BGFX_STATE_DEPTH_TEST_LESS;
	static constexpr uint64_t stateDepthTestLessEqual = BGFX_STATE_DEPTH_TEST_LEQUAL;
	static constexpr uint64_t stateDepthTestGreater = BGFX_STATE_DEPTH_TEST_GREATER;
	static constexpr uint64_t stateDepthTestGreaterEqual = BGFX_STATE_DEPTH_TEST_GEQUAL;

	static constexpr uint64_t stateCullCw = BGFX_STATE_CULL_CW;
	static constexpr uint64_t stateCullCcw = BGFX_STATE_CULL_CCW;

	static constexpr uint64_t stateLines = BGFX_STATE_PT_LINES;

	static constexpr uint64_t stateBlendAlpha = BGFX_STATE_BLEND_ALPHA;
	static constexpr uint64_t stateBlendAdd = BGFX_STATE_BLEND_ADD;

	static constexpr uint64_t stateMsaa = BGFX_STATE_MSAA;

	// view modes
	static constexpr int sequential = bgfx::ViewMode::Sequential;
	static constexpr int depthAscending = bgfx::ViewMode::DepthAscending;
	static constexpr int depthDescending = bgfx::ViewMode::DepthDescending;

	// access flags
	static constexpr int readAccess = bgfx::Access::Read;
	static constexpr int writeAccess = bgfx::Access::Write;
	static constexpr int readWriteAccess = bgfx::Access::ReadWrite;

	static constexpr uint8_t discardNone = BGFX_DISCARD_NONE;
	static constexpr uint8_t discardBindings = BGFX_DISCARD_BINDINGS;
	static constexpr uint8_t discardIndexBuffer = BGFX_DISCARD_INDEX_BUFFER;
	static constexpr uint8_t discardInstanceData = BGFX_DISCARD_INSTANCE_DATA;
	static constexpr uint8_t discardState = BGFX_DISCARD_STATE;
	static constexpr uint8_t discardTransform = BGFX_DISCARD_TRANSFORM;
	static constexpr uint8_t discardVertexStreams = BGFX_DISCARD_VERTEX_STREAMS;
	static constexpr uint8_t discardAll = BGFX_DISCARD_ALL;

	// constructor
	OtPass();

	// handle rendering pass
	void setClear(bool color, bool depth=false, const glm::vec4& rgba=glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), float depthValue=1.0f);
	inline void setRectangle(int x, int y, int w, int h) { 	bgfx::setViewRect(view, static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(w), static_cast<uint16_t>(h)); }
	inline void setFrameBuffer(bgfx::FrameBufferHandle framebuffer) { bgfx::setViewFrameBuffer(view, framebuffer); }
	inline void setFrameBuffer(OtFrameBuffer& framebuffer) { framebuffer.submit(view); }
	inline void setFrameBuffer(OtGbuffer& gbuffer) { gbuffer.submit(view); }
	inline void setTransform(const glm::mat4& viewTransform, const glm::mat4& projectionTransform) { bgfx::setViewTransform(view, glm::value_ptr(viewTransform), glm::value_ptr(projectionTransform)); }
	inline void setViewMode(int mode) { bgfx::setViewMode(view, bgfx::ViewMode::Enum(mode)); }
	inline void setState(uint64_t state) { bgfx::setState(state); }
	inline void setTransform(const glm::mat4& transform) { bgfx::setTransform(glm::value_ptr(transform)); }
	inline void setTransforms(const glm::mat4* transforms, size_t count) { bgfx::setTransform(transforms, static_cast<uint16_t>(count)); }
	inline void setVertexCount(size_t count) { bgfx::setVertexCount(static_cast<uint32_t>(count)); }
	inline void setInstanceCount(size_t count) { bgfx::setInstanceCount(static_cast<uint32_t>(count)); }

	void submitQuad(int w, int h);

	inline void touch() { bgfx::touch(view); }
	inline void discard(uint8_t flags=discardAll) { bgfx::discard(flags); }

	inline void runShaderProgram(OtShaderProgram& program, uint8_t discard=discardAll) {
		program.dispatch(view, discard);
	}

	// handle compute pass
	inline void setImage(int stage, OtTexture& texture, int mip, int access) {
		bgfx::setImage(static_cast<uint8_t>(stage), texture.getHandle(), static_cast<uint8_t>(mip), bgfx::Access::Enum(access));
	}

	void setImage(int stage, OtCubeMap& cubemap, int mip, int access) {
		bgfx::setImage(static_cast<uint8_t>(stage), cubemap.getHandle(), static_cast<uint8_t>(mip), bgfx::Access::Enum(access));
	}

	inline void runComputeProgram(OtComputeProgram& program, uint32_t x, uint32_t y, uint32_t z, uint8_t discard=discardAll) {
		program.dispatch(view, x, y, z, discard);
	}

	// handle blit pass
	inline void blit(
		bgfx::TextureHandle dest,
		uint16_t dx, uint16_t dy,
		bgfx::TextureHandle src,
		uint16_t sx=0, uint16_t sy=0,
		uint16_t sw=UINT16_MAX, uint16_t sh=UINT16_MAX) {

		bgfx::blit(view, dest, dx, dy, src, sx, sy, sw, sh);
	}

	inline void blit(
		OtTexture& dest,
		uint16_t dx, uint16_t dy,
		OtTexture& src,
		uint16_t sx=0, uint16_t sy=0,
		uint16_t sw=UINT16_MAX, uint16_t sh=UINT16_MAX) {

		blit(dest.getHandle(), dx, dy, src.getHandle(), sx, sy, sw, sh);
	}

	inline void blit(OtTexture& dest, OtTexture& src) {
		blit(dest, 0, 0, src);
	}

	// get view ID
	inline bgfx::ViewId getViewId() { return view; }

private:
	// properties
	bgfx::ViewId view = 0;
};


//
//	Functions
//

// reset rendering pass system (to be run each frame)
void OtPassReset();

// get view count
int OtPassCount();
