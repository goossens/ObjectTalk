//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"

#include "glm/glm.hpp"

#include "OtBloom.h"
#include "OtBloomShader.h"
#include "OtFramework.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBloomVS),
	BGFX_EMBEDDED_SHADER(OtBloomFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBloomClass::OtBloomClass
//

OtBloomClass::OtBloomClass() {
	// setup uniforms
	auto framework = OtFrameworkClass::instance();
	bloomUniform = framework->getUniform("u_bloom", bgfx::UniformType::Vec4, 1);

	// initialize shader
	shader = framework->getProgram(embeddedShaders, "OtBloomVS", "OtBloomFS");
}


//
//	OtBloomClass::execute
//

void OtBloomClass::execute(int view, int w, int h) {
	glm::vec4 uniforms = glm::vec4(0.2 / (float) w, 0.2 / (float) h, 2.0, 1.0);
	bgfx::setUniform(bloomUniform, &uniforms, 1);
	bgfx::submit(view, shader);
}
