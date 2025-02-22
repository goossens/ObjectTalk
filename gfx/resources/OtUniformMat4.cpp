//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtUniformMat4.h"


//
//	OtUniformMat4::OtUniformMat4
//

OtUniformMat4::OtUniformMat4(const char* name, size_t size) {
	initialize(name, size);
}


//
//	OtUniformMat4::~OtUniformMat4
//

OtUniformMat4::~OtUniformMat4() {
	clear();
}


//
//	OtUniformMat4::initialize
//

void OtUniformMat4::initialize(const char* n, size_t s) {
	// clear first if (required)
	if (values || isValid()) {
		clear();
	}

	// store properties and allocate memory
	name = n;
	size = s;
	values = new glm::mat4[size];
}


//
//	OtUniformMat4::clear
//

void OtUniformMat4::clear() {
	// release values (if required)
	if (values) {
		delete [] values;
		values = nullptr;
	}

	// release uniform (if required)
	if (isValid()) {
		uniform.clear();
	}

	// reset properties
	name.clear();
	size = 0;
}


//
//	OtUniformMat4::submit
//

void OtUniformMat4::submit() {
	// generate resource (if required)
	if (!isValid()) {
		// sanity check
		if (name.size() && size) {
			// register uniform
			uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Mat4, static_cast<uint16_t>(size));

		} else {
			OtLogFatal("Internal error: uniform not initialized before submission");
		}
	}

	// submit uniform value(s) to GPU
	bgfx::setUniform(uniform.getHandle(), values, static_cast<uint16_t>(size));
}
