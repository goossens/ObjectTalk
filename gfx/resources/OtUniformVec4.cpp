//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtUniformVec4.h"


//
//	OtUniformVec4::OtUniformVec4
//

OtUniformVec4::OtUniformVec4(const char* name, size_t size) {
	initialize(name, size);
}


//
//	OtUniformVec4::~OtUniformVec4
//

OtUniformVec4::~OtUniformVec4() {
	clear();
}


//
//	OtUniformVec4::initialize
//

void OtUniformVec4::initialize(const char* n, size_t s) {
	// clear first
	clear();

	// store properties and allocate memory
	name = n;
	size = s;
	values = new glm::vec4[size];
}


//
//	OtUniformVec4::clear
//

void OtUniformVec4::clear() {
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
//	OtUniformVec4::submit
//

void OtUniformVec4::submit() {
	// generate resource (if required)
	if (!isValid()) {
		// sanity check
		if (name.size() && size) {
			// register uniform
			uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Vec4, static_cast<uint16_t>(size));

		} else {
			OtLogFatal("Internal error: uniform not initialized before submission");
		}
	}

	// submit uniform value(s) to GPU
	bgfx::setUniform(uniform.getHandle(), values, static_cast<uint16_t>(size));
}
