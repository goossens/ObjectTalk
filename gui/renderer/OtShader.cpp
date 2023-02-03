//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"

#include "OtException.h"

#include "OtShader.h"
#include "OtShaders.h"


//
//	OtShader::OtShader
//

OtShader::OtShader(const char* vertex, const char* fragment) {
	initialize(vertex, fragment);
}


//
//	OtShader::initialize
//

void OtShader::initialize(const char* vertex, const char* fragment) {
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, vertex),
		bgfx::createEmbeddedShader(embeddedShaders, type, fragment),
		true);
}


//
//	OtShader::setState
//

void OtShader::setState(states state) {
	if (state == noDepth) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_MSAA);

	} else if (state == wireframe) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA |
			BGFX_STATE_PT_LINES |
			BGFX_STATE_LINEAA |
			BGFX_STATE_BLEND_ALPHA);

	} else if (state == noCulling) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA);

	} else if (state == cullFrontFace) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA |
			BGFX_STATE_CULL_CCW);

	} else if (state == cullBackFace) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA |
			BGFX_STATE_CULL_CW);

	} else if (state == blendAlpha) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA |
			BGFX_STATE_BLEND_ALPHA);

	} else if (state == blendAdditive) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_WRITE_Z |
			BGFX_STATE_DEPTH_TEST_LESS |
			BGFX_STATE_MSAA |
			BGFX_STATE_BLEND_ADD);

	} else if (state == filter) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A);

	} else if (state == filterAdditive) {
		bgfx::setState(
			BGFX_STATE_WRITE_RGB |
			BGFX_STATE_WRITE_A |
			BGFX_STATE_BLEND_ADD);
	}
}


//
//	OtShader::setTransform
//

void OtShader::setTransform(const glm::mat4 &transform) {
	bgfx::setTransform(glm::value_ptr(transform));
}


//
//	OtShader::submit
//

void OtShader::submit(bgfx::ViewId view) {
	if (isValid()) {
		bgfx::submit(view, shader.getHandle());

	} else {
		OtExcept("Internal error: Shader not initialized before submission");
	}
}
