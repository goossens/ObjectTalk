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

#include "OtBlit.h"
#include "OtBlitShader.h"
#include "OtFramework.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBlitVS),
	BGFX_EMBEDDED_SHADER(OtBlitFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBlitClass::OtBlitClass
//

OtBlitClass::OtBlitClass() {
	// setup uniforms
	auto framework = OtFrameworkClass::instance();
	blitUniform = framework->getUniform("u_blit", bgfx::UniformType::Vec4, 1);

	// initialize shader
	shader = framework->getProgram(embeddedShaders, "OtBlitVS", "OtBlitFS");
}


//
//	OtBlitClass::execute
//

void OtBlitClass::execute(int view, int w, int h) {
	glm::vec4 uniforms = glm::vec4(intensity == -1.0 ? 1.0 : intensity, alpha == -1.0 ? 1.0 : alpha, 0.0, 0.0);
	bgfx::setUniform(blitUniform, &uniforms, 1);
	bgfx::submit(view, shader);
}
