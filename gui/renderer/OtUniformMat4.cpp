//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/ext.hpp"

#include "OtException.h"

#include "OtUniformMat4.h"


//
//	OtUniformMat4::OtUniformMat4
//

OtUniformMat4::OtUniformMat4(const char *name, size_t size) {
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

void OtUniformMat4::initialize(const char *name, size_t s) {
	// clear uniform
	clear();

	// remember size
	size = s;

	// register uniform
	uniform = bgfx::createUniform(name, bgfx::UniformType::Mat4, size);

	// allocate space for uniform values
	values = new glm::mat4[size];
}


//
//	OtUniformMat4::clear
//

void OtUniformMat4::clear() {
		// release values
	if (values) {
		delete [] values;
		values = nullptr;
	}

	// release uniform
	if (bgfx::isValid(uniform)) {
		bgfx::destroy(uniform);
		uniform = BGFX_INVALID_HANDLE;
	}
}


//
//	OtUniformMat4::submit
//

void OtUniformMat4::submit() {
	// sanity check
	if (!size) {
		OtExcept("Internal error: Uniform not initialized");
	}

	// submit uniform to GPU
	bgfx::setUniform(uniform, values, size);
}
