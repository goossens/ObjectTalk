//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bloom.h"
#include "bloomshader.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_bloom),
	BGFX_EMBEDDED_SHADER(fs_bloom),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBloomClass::OtBloomClass
//

OtBloomClass::OtBloomClass() {
	// setup uniforms
	bloomUniform = bgfx::createUniform("u_bloom", bgfx::UniformType::Vec4, 1);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_bloom"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_bloom"),
		true);
}


//
//	OtBloomClass::~OtBloomClass
//

OtBloomClass::~OtBloomClass() {
	bgfx::destroy(bloomUniform);
	bgfx::destroy(shader);
}


//
//	OtBloomClass::execute
//

void OtBloomClass::execute(int view, int w, int h) {
	glm::vec4 uniforms = glm::vec4(0.2 / (float) w, 0.2 / (float) h, 2.0, 1.0);
	bgfx::setUniform(bloomUniform, &uniforms, 1);
	bgfx::submit(view, shader);
}
