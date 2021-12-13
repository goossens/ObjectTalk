//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "application.h"
#include "blit.h"
#include "blitshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_blit),
	BGFX_EMBEDDED_SHADER(fs_blit),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBlitClass::OtBlitClass
//

OtBlitClass::OtBlitClass() {
	// setup uniforms
	blitUniform = bgfx::createUniform("u_blit", bgfx::UniformType::Vec4, 1);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_blit"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_blit"),
		true);
}


//
//	OtBlitClass::~OtBlitClass
//

OtBlitClass::~OtBlitClass() {
	bgfx::destroy(blitUniform);
	bgfx::destroy(shader);
}


//
//	OtBlitClass::execute
//

void OtBlitClass::execute(int view, int w, int h) {
	glm::vec4 uniforms = glm::vec4(intensity == -1.0 ? 1.0 : intensity, alpha == -1.0 ? 1.0 : alpha, 0.0, 0.0);
	bgfx::setUniform(blitUniform, &uniforms, 1);
	bgfx::submit(view, shader);
}
