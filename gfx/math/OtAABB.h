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


//
//	OtAABB (Axis-Aligned Bounding Box)
//

class OtAABB {
public:
	// constructor
	OtAABB();
	inline OtAABB(const glm::vec3& mina, const glm::vec3& maxa) : minp(mina), maxp(maxa) {}

	inline OtAABB(float minx, float miny, float minz, float maxx, float maxy, float maxz)
		: minp(glm::vec3(minx, miny, minz)), maxp(glm::vec3(maxx, maxy, maxz)) {}

	// clear the bounding box
	void clear();

	// add a point to the bounding box
	void addPoint(const glm::vec3& point);

	// add another AABB
	void addAABB(const OtAABB& aabb);

	// does it overlap with another AABB
	bool overlaps(const OtAABB& aabb);

	// get min/max values
	inline const glm::vec3& getMin() const { return minp; }
	inline const glm::vec3& getMax() const { return maxp; }

	inline glm::vec3& getMin() { return minp; }
	inline glm::vec3& getMax() { return maxp; }

	inline glm::vec3 getSize() { return maxp - minp; }
	inline glm::vec3 getCenter() { return (minp + maxp) / 2.0f; }
	inline float getRadius() { return glm::length(minp - getCenter()); }

	// transform bounding box
	OtAABB transform(const glm::mat4& matrix);

	// see if AABBs are identical
	inline bool operator==(OtAABB& rhs) { return minp == rhs.minp && maxp == rhs.maxp; }
	inline bool operator!=(OtAABB& rhs) { return !operator==(rhs); }
	inline bool operator==(const OtAABB& rhs) const { return minp == rhs.minp && maxp == rhs.maxp; }
	inline bool operator!=(const OtAABB& rhs) const { return !operator==(rhs); }

	// debugging support
	void debug();

private:
	// properties
	glm::vec3 minp;
	glm::vec3 maxp;
};
