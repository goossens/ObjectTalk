//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtUniformMat3.h"


//
//	OtUniformMat3::OtUniformMat3
//

OtUniformMat3::OtUniformMat3(const char* name, size_t size) {
	initialize(name, size);
}


//
//	OtUniformMat3::~OtUniformMat3
//

OtUniformMat3::~OtUniformMat3() {
	clear();
}


//
//	OtUniformMat3::initialize
//

void OtUniformMat3::initialize(const char* n, size_t s) {
	// clear first if (required)
	if (values || isValid()) {
		clear();
	}

	// store properties and allocate memory
	name = n;
	size = s;
	values = new glm::mat3[size];
}


//
//	OtUniformMat3::clear
//

void OtUniformMat3::clear() {
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
//	OtUniformMat3::submit
//

void OtUniformMat3::submit() {
	// generate resource (if required)
	if (!isValid()) {
		// sanity check
		if (name.size() && size) {
			// register uniform
			uniform = bgfx::createUniform(name.c_str(), bgfx::UniformType::Mat3, size);

		} else {
			OtLogFatal("Internal error: uniform not initialized before submission");
		}
	}

	// submit uniform value(s) to GPU
	bgfx::setUniform(uniform.getHandle(), values, size);
}
