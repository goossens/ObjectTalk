//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"

#include "OtUniform.h"


//
//	OtUniform::OtUniform
//

OtUniform::OtUniform(const char *name, size_t s) {
	// remember size
	size = s;

	// register uniform
	uniform = bgfx::createUniform(name, bgfx::UniformType::Vec4, size);

	// allocate space for uniform values
	values = new glm::vec4[size];
}


//
//	OtUniform::~OtUniform
//

OtUniform::~OtUniform() {
	clear();
}


//
//	OtUniform::initialize
//

void OtUniform::initialize(const char *name, size_t s) {
	// remember size
	size = s;

	// register uniform
	uniform = bgfx::createUniform(name, bgfx::UniformType::Vec4, size);

	// allocate space for uniform values
	values = new glm::vec4[size];
}


//
//	OtUniform::clear
//

void OtUniform::clear() {
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
//	OtUniform::submit
//

void OtUniform::submit() {
	// sanity check
	if (!size) {
		OtExcept("Internal error: Uniform not initialized");
	}

	// submit uniform to GPU
	bgfx::setUniform(uniform, values, size);
}
