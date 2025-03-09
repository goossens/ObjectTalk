//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	// indices for planes and corners
	static constexpr size_t leftPlane = 0;
	static constexpr size_t rightPlane = 1;
	static constexpr size_t bottomPlane = 2;
	static constexpr size_t topPlane = 3;
	static constexpr size_t nearPlane = 4;
	static constexpr size_t farPlane = 5;
	static constexpr size_t planeCount = 6;

	static constexpr size_t nearBottomLeft = 0;
	static constexpr size_t nearTopLeft = 1;
	static constexpr size_t nearTopRight = 2;
	static constexpr size_t nearBottomRight = 3;

	static constexpr size_t farBottomLeft = 4;
	static constexpr size_t farTopLeft = 5;
	static constexpr size_t farTopRight = 6;
	static constexpr size_t farBottomRight = 7;
	static constexpr size_t cornerCount = 8;

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
	inline glm::vec3 getCorner(size_t corner) const { return corners[corner]; }

	// get specified plane
	inline OtPlane getPlane(size_t plane) const { return planes[plane]; }

	// get AABB (Axis-Alligned bounding box)
	OtAABB getAABB();

	// debugging support
	void debugPlanes();
	void debugPoints();

private:
	std::array<OtPlane, planeCount> planes;
	std::array<glm::vec3, cornerCount> corners;
};
