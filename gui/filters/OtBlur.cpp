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

#include "OtBlur.h"
#include "OtBlurShader.h"
#include "OtFramework.h"


//
//	Globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtBlurVS),
	BGFX_EMBEDDED_SHADER(OtBlurFS),
	BGFX_EMBEDDED_SHADER_END()
};


//
//	OtBlurClass::OtBlurClass
//

OtBlurClass::OtBlurClass() {
	// setup uniforms
	auto framework = OtFrameworkClass::instance();
	blurUniform = framework->getUniform("u_blur", bgfx::UniformType::Vec4, 1);

	// initialize shader
	shader = framework->getProgram(embeddedShaders, "OtBlurVS", "OtBlurFS");
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
