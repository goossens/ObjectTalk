//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
	glm::vec3 getMin() const { return minp; }
	glm::vec3 getMax() const { return maxp; }

	// transform bounding box
	OtAABB transform(const glm::mat4& matrix);

private:
	glm::vec3 minp;
	glm::vec3 maxp;
};
