//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFormat.h"
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

			auto vertexShader = bgfx::createEmbeddedShader(embeddedShaders, type, vertexShaderName.c_str());
			auto fragmentShader = bgfx::createEmbeddedShader(embeddedShaders, type, fragmentShaderName.c_str());
			program = bgfx::createProgram(vertexShader, fragmentShader, true);

			if (!bgfx::isValid(vertexShader)) {
				OtLogFatal(OtFormat("Internal error: Unknown vertex shader [%s]", vertexShaderName.c_str()));
			}

			if (!bgfx::isValid(fragmentShader)) {
				OtLogFatal(OtFormat("Internal error: Unknown vertex shader [%s]", fragmentShaderName.c_str()));
			}

			if (!isValid()) {
				OtLogFatal("Internal error: Can't create shader program");
			}

		} else {
			OtLogFatal("Internal error: Shader program not initialized before submission");
		}
	}

	bgfx::submit(view, program.getHandle());
}
