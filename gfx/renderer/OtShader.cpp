//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

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
	vertexShaderName = vertex;
	fragmentShaderName = fragment;
}


//
//	OtShader::submit
//

void OtShader::submit(bgfx::ViewId view) {
	if (!isValid()) {
		if (vertexShaderName.size() && fragmentShaderName.size()) {
			bgfx::RendererType::Enum type = bgfx::getRendererType();

			shader = bgfx::createProgram(
				bgfx::createEmbeddedShader(embeddedShaders, type, vertexShaderName.c_str()),
				bgfx::createEmbeddedShader(embeddedShaders, type, fragmentShaderName.c_str()),
				true);

		} else {
			OtExcept("Internal error: Shader not initialized before submission");
		}
	}

	bgfx::submit(view, shader.getHandle());
}
