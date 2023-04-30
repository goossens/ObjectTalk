//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtBgfxHandle.h"


//
//	OtUniformVec4
//

class OtUniformVec4 {
public:
	// constructors/destructor
	OtUniformVec4() = default;
	OtUniformVec4(const char* name, size_t size=1);
	OtUniformVec4(const OtUniformVec4&) = delete; // no copy constructor
	OtUniformVec4& operator=(const OtUniformVec4&) = delete; // no copy assignment
	OtUniformVec4(OtUniformVec4&&) = default;
	OtUniformVec4& operator=(OtUniformVec4&&) = default;
	~OtUniformVec4();

	// initialize uniform
	void initialize(const char* name, size_t size=1);

	// clear the resources
	void clear();

	// see if sampler is valid
	bool isValid() { return uniform.isValid(); }

	// get access to values
	glm::vec4* getValues() { return values; }
	size_t getValueCount() { return size; }

	// set uniform value(s)
	inline void set(size_t index, const glm::vec4& value) { values[index] = value; }

	// submit uniform to GPU
	void submit();

private:
	// number of uniform values
	size_t size = 0;

	// handle for uniform
	OtBgfxHandle<bgfx::UniformHandle> uniform;

	// actual values
	glm::vec4* values = nullptr;
};
