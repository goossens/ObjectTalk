//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "glm/glm.hpp"

#include "OtAABB.h"
#include "OtPlane.h"


//
//	OtFrustum
//

class OtFrustum {
public:
	enum {
		leftPlane = 0,
		rightPlane,
		bottomPlane,
		topPlane,
		nearPlane,
		farPlane,
		planeCount
	};

	enum {
		nearBottomLeft = 0,
		nearTopLeft,
		nearTopRight,
		nearBottomRight,
		farBottomLeft,
		farTopLeft,
		farTopRight,
		farBottomRight,
		pointCount
	};

	// constructors
	OtFrustum();
	OtFrustum(const glm::mat4& matrix);

	// see if a point is visible
	bool isVisiblePoint(const glm::vec3& point);

	// see if an AABB box is visible
	bool isVisibleAABB(const OtAABB& aabb);

	// see if a sphere is visible
	bool isVisibleSphere(const glm::vec3& center, float radius);

	// get center of frustum
	glm::vec3 getCenter();

	// get specified corner
	inline glm::vec3 getCorner(int point) const { return points[point]; }

	// get specified plane
	inline OtPlane getPlane(int plane) const { return planes[plane]; }

	// get AABB (Axis-Alligned bounding box)
	OtAABB getAABB();

	// debugging support
	void debugPlanes();
	void debugPoints();

private:
	std::array<OtPlane, planeCount> planes;
	std::array<glm::vec3, pointCount> points;
};
