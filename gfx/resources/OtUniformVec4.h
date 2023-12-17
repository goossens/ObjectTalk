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
	OtUniformVec4() = delete;
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
	inline void setValue(size_t index, const glm::vec4& value) { values[index] = value; }
	inline void setValue(size_t index, const glm::vec3& v1, float v2) { values[index] = glm::vec4(v1, v2); }
	inline void setValue(size_t index, float v1, const glm::vec3& v2) { values[index] = glm::vec4(v1, v2); }
	inline void setValue(size_t index, const glm::vec2& v1, const glm::vec2& v2) { values[index] = glm::vec4(v1, v2); }
	inline void setValue(size_t index, const glm::vec2& v1, float v2, float v3) { values[index] = glm::vec4(v1, v2, v3); }
	inline void setValue(size_t index, float v1, float v2,const glm::vec2& v3) { values[index] = glm::vec4(v1, v2, v3); }
	inline void setValue(size_t index, float v1, const glm::vec2& v2, float v3) { values[index] = glm::vec4(v1, v2, v3); }
	inline void setValue(size_t index, float v1, float v2, float v3, float v4) { values[index] = glm::vec4(v1, v2, v3, v4); }
	inline glm::vec4 getValue(size_t index) { return values[index]; }
	inline glm::vec4* getValues() { return values; }
	inline size_t getValueCount() { return size; }

	// submit uniform to GPU
	void submit();

private:
	// properties
	std::string name;
	size_t size = 0;

	// handle for uniform
	OtBgfxHandle<bgfx::UniformHandle> uniform;

	// actual values
	glm::vec4* values = nullptr;
};
