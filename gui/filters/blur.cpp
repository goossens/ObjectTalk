//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "blur.h"
#include "blurshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_blur),
	BGFX_EMBEDDED_SHADER(fs_blur),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBlurClass::OtBlurClass
//

OtBlurClass::OtBlurClass() {
	// setup uniforms
	blurUniform = bgfx::createUniform("u_blur", bgfx::UniformType::Vec4, 1);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_blur"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_blur"),
		true);
}


//
//	OtBlurClass::~OtBlurClass
//

OtBlurClass::~OtBlurClass() {
	bgfx::destroy(blurUniform);
	bgfx::destroy(shader);
}


//
//	OtBlurClass::execute
//

void OtBlurClass::execute(int view, int w, int h) {
	glm::vec4 uniforms = glm::vec4(
		horizontalScale == -1.0 ? (0.2 / (float) w) : horizontalScale,
		verticalScale == -1.0 ? (0.2 / (float) h) : verticalScale,
		intensity == -1.0 ? 2.0 : intensity,
		alpha == -1.0 ? 1.0 : alpha);

	bgfx::setUniform(blurUniform, &uniforms, 1);
	bgfx::submit(view, shader);
}
