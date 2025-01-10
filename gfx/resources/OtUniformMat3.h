//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtUniformMat3
//

class OtUniformMat3 {
public:
	// constructors/destructor
	OtUniformMat3() = default;
	OtUniformMat3(const char* name, size_t size=1);
	OtUniformMat3(const OtUniformMat3&) = delete; // no copy constructor
	OtUniformMat3& operator=(const OtUniformMat3&) = delete; // no copy assignment
	OtUniformMat3(OtUniformMat3&&) = default;
	OtUniformMat3& operator=(OtUniformMat3&&) = default;
	~OtUniformMat3();

	// initialize uniform
	void initialize(const char* name, size_t size=1);

	// clear the resources
	void clear();

	// see if unifirm is valid
	inline bool isValid() { return uniform.isValid(); }

	// get access to values
	inline void setValue(size_t index, const glm::mat3& value) { values[index] = value; }
	inline glm::mat3 getValue(size_t index) { return values[index]; }
	inline glm::mat3* getValues() { return values; }
	inline size_t getValueCount() { return size; }

	// set uniform value(s)
	inline void set(size_t index, const glm::mat3& value) { values[index] = value; }

	// submit uniform to GPU
	void submit();

private:
	// properties
	std::string name;
	size_t size = 0;

	// handle for uniform
	OtBgfxHandle<bgfx::UniformHandle> uniform;

	// actual values
	glm::mat3* values = nullptr;
};
