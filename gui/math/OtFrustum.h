//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

	// initialize the frustum based on provided points
	OtFrustum(
		glm::vec3& nearBottomLeft,
		glm::vec3& nearTopLeft,
		glm::vec3& nearTopRight,
		glm::vec3& nearBottomRight,
		glm::vec3& farBottomLeft,
		glm::vec3& farTopLeft,
		glm::vec3& farTopRight,
		glm::vec3& farBottomRight);

	// see if a point is visible
	bool isVisiblePoint(const glm::vec3& point);

	// see if an AABB box is visible
	bool isVisibleAABB(const OtAABB& aabb);

	// see if a sphere is visible
	bool isVisibleSphere(const glm::vec3& center, float radius);

	// get center of frustum
	glm::vec3 getCenter();

	// get specified corner
	glm::vec3 getCorner(int point) const { return points[point]; }

	// get specified plane
	OtPlane getPlane(int plane) const { return planes[plane]; }

	// get AABB (Axis-Alligned bounding box)
	OtAABB getAABB();

	// transform frustum
	OtFrustum transform(const glm::mat4& transform);

	// debugging support
	void debugPlanes();
	void debugPoints();

private:
	std::array<OtPlane, planeCount> planes;
	std::array<glm::vec3, pointCount> points;
};
