//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <limits>

#include "bgfx/bgfx.h"
#include "debugdraw.h"
#include "glm/glm.hpp"

#include "OtMath.h"


//
//	OtAABB (Axis-Alligned bounding box)
//

class OtAABBClass;
typedef std::shared_ptr<OtAABBClass> OtAABB;

class OtAABBClass : public OtMathClass {
public:
	// constructor
	OtAABBClass() {
		clear();
	}

	// clear the bounding box
	void clear() {
		float minv = std::numeric_limits<float>::lowest();
		float maxv = std::numeric_limits<float>::max();

		minp = glm::vec3(maxv, maxv, maxv);
		maxp = glm::vec3(minv, minv, minv);
	}

	// add a point to the bounding box
	void addPoint(const glm::vec3& point) {
		minp.x = std::min(minp.x, point.x);
		minp.y = std::min(minp.y, point.y);
		minp.z = std::min(minp.z, point.z);
		maxp.x = std::max(maxp.x, point.x);
		maxp.y = std::max(maxp.y, point.y);
		maxp.z = std::max(maxp.z, point.z);
	}

	// get min/max values
	glm::vec3 getMin() { return minp; }
	glm::vec3 getMax() { return maxp; }

	// transform bounding box
	OtAABB transform(const glm::mat4& matrix) {
		glm::vec3 points[8] = {
			 { minp.x, minp.y, minp.z },
			 { minp.x, minp.y, maxp.z },
			 { minp.x, maxp.y, maxp.z },
			 { minp.x, maxp.y, minp.z },

			 { maxp.x, minp.y, minp.z },
			 { maxp.x, minp.y, maxp.z },
			 { maxp.x, maxp.y, maxp.z },
			 { maxp.x, maxp.y, minp.z }
		};

		OtAABB aabb = create();

		for (auto i = 0; i < 6; i++) {
			aabb->addPoint(points[i]);
		}

		return aabb;
	}

	// render the bounding box for debugging purposes
	void render(DebugDrawEncoder* debugDraw) {
		glm::vec3 points[8] = {
			 { minp.x, minp.y, minp.z },
			 { minp.x, minp.y, maxp.z },
			 { minp.x, maxp.y, maxp.z },
			 { minp.x, maxp.y, minp.z },
			 { maxp.x, minp.y, minp.z },
			 { maxp.x, minp.y, maxp.z },
			 { maxp.x, maxp.y, maxp.z },
			 { maxp.x, maxp.y, minp.z }
		};

		debugDraw->setColor(0xff00ff00);

		debugDraw->moveTo(points[0].x, points[0].y, points[0].z);
		debugDraw->lineTo(points[1].x, points[1].y, points[1].z);
		debugDraw->lineTo(points[2].x, points[2].y, points[2].z);
		debugDraw->lineTo(points[3].x, points[3].y, points[3].z);
		debugDraw->close();

		debugDraw->moveTo(points[4].x, points[4].y, points[4].z);
		debugDraw->lineTo(points[5].x, points[5].y, points[5].z);
		debugDraw->lineTo(points[6].x, points[6].y, points[6].z);
		debugDraw->lineTo(points[7].x, points[7].y, points[7].z);
		debugDraw->close();

		debugDraw->moveTo(points[0].x, points[0].y, points[0].z);
		debugDraw->lineTo(points[4].x, points[4].y, points[4].z);

		debugDraw->moveTo(points[1].x, points[1].y, points[1].z);
		debugDraw->lineTo(points[5].x, points[5].y, points[5].z);

		debugDraw->moveTo(points[2].x, points[2].y, points[2].z);
		debugDraw->lineTo(points[6].x, points[6].y, points[6].z);

		debugDraw->moveTo(points[3].x, points[3].y, points[3].z);
		debugDraw->lineTo(points[7].x, points[7].y, points[7].z);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type;

		if (!type) {
			type = OtTypeClass::create<OtAABBClass>("AABB", OtMathClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtAABB create() {
		OtAABB aabb = std::make_shared<OtAABBClass>();
		aabb->setType(getMeta());
		return aabb;
	}

private:
	glm::vec3 minp = glm::vec3(0.0);
	glm::vec3 maxp = glm::vec3(0.0);
};
