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


//
//	OtAABB (Axis-Alligned bounding box)
//

class OtAABB {
public:
	// constructor
	OtAABB();

	// clear the bounding box
	void clear();

	// add a point to the bounding box
	void addPoint(const glm::vec3& point);

	// get min/max values
	const glm::vec3& getMin() const { return minp; }
	const glm::vec3& getMax() const { return maxp; }

	glm::vec3& getMin() { return minp; }
	glm::vec3& getMax() { return maxp; }

	// transform bounding box
	OtAABB transform(const glm::mat4& matrix);

	// debugging support
	void debug();

private:
	glm::vec3 minp;
	glm::vec3 maxp;
};
