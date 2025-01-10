//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
			auto vertexShader = OtShaders::get(vertexShaderName);
			auto fragmentShader = OtShaders::get(fragmentShaderName);
			program = bgfx::createProgram(vertexShader, fragmentShader, true);

			if (!isValid()) {
				OtLogFatal("Internal error: can't create shader program");
			}

		} else {
			OtLogFatal("Internal error: shader program not initialized before submission");
		}
	}

	bgfx::submit(view, program.getHandle());
}
