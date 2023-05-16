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
	OtAABB(const glm::vec3& mina, const glm::vec3& maxa) : minp(mina), maxp(maxa) {}

	OtAABB(float minx, float miny, float minz, float maxx, float maxy, float maxz)
		: minp(glm::vec3(minx, miny, minz)), maxp(glm::vec3(maxx, maxy, maxz)) {}

	// clear the bounding box
	void clear();

	// add a point to the bounding box
	void addPoint(const glm::vec3& point);

	// add another AABB
	void addAABB(const OtAABB& aabb);

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
