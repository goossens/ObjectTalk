//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"


//
//	OtUniformMat4
//

class OtUniformMat4 {
public:
	// constructors/destructor
	OtUniformMat4() = default;
	OtUniformMat4(const char* name, size_t size=1);
	~OtUniformMat4();

	// initialize uniform
	void initialize(const char* name, size_t size=1);

	// clear the resources
	void clear();

	// get access to values
	glm::mat4* getValues() { return values; }
	size_t getValueCount() { return size; }

	// set uniform value(s)
	inline void set(size_t index, const glm::mat4& value) { values[index] = value; }

	// submit uniform to GPU
	void submit();

private:
	// number of uniform values
	size_t size = 0;

	// handle for uniform
	bgfx::UniformHandle uniform = BGFX_INVALID_HANDLE;

	// actual values
	glm::mat4* values = nullptr;
};
