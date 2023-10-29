//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtShaderProgram.h"
#include "OtShaders.h"


//
//	OtShaderProgram::OtShaderProgram
//

OtShaderProgram::OtShaderProgram(const char* vertex, const char* fragment) {
	initialize(vertex, fragment);
}


//
//	OtShaderProgram::initialize
//

void OtShaderProgram::initialize(const char* vertex, const char* fragment) {
	vertexShaderName = vertex;
	fragmentShaderName = fragment;
}


//
//	OtShaderProgram::submit
//

void OtShaderProgram::submit(bgfx::ViewId view) {
	if (!isValid()) {
		if (vertexShaderName.size() && fragmentShaderName.size()) {
			bgfx::RendererType::Enum type = bgfx::getRendererType();

			program = bgfx::createProgram(
				bgfx::createEmbeddedShader(embeddedShaders, type, vertexShaderName.c_str()),
				bgfx::createEmbeddedShader(embeddedShaders, type, fragmentShaderName.c_str()),
				true);

		} else {
			OtLogFatal("Internal error: Shader program not initialized before submission");
		}
	}

	bgfx::submit(view, program.getHandle());
}
