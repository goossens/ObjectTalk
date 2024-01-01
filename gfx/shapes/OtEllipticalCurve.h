//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtNumbers.h"

#include "OtCurve.h"


//
//	OtEllipticalCurveClass
//

class OtEllipticalCurveClass;
typedef std::shared_ptr<OtEllipticalCurveClass> OtEllipticalCurve;

class OtEllipticalCurveClass : public OtCurveClass {
public:
	// constructor
	inline OtEllipticalCurveClass(const glm::vec2& c, float xr, float yr, float sa=0.0f, float ea=std::numbers::pi2, bool cw=false, float r=0.0f) :
		center(c), xRadius(xr), yRadius(yr), startAngle(sa), endAngle(ea), clockwise(cw), rotation(r) {
		delta = endAngle - startAngle;

		while (delta < 0.0f) {
			delta += std::numbers::pi2;
		}

		while (delta > std::numbers::pi2) {
			delta -= std::numbers::pi2;
		}

		if (clockwise && delta != std::numbers::pi2) {
			delta -= std::numbers::pi2;
		}
	}

	// get a point on the curve at t [0, 1]
	inline glm::vec2 getPoint(float t) override {
		float angle = startAngle + t * delta;
		float x = center.x + xRadius * std::cos(angle);
		float y = center.y + yRadius * std::sin(angle);

		if (rotation) {
			float tx = x - center.x;
			float ty = y - center.y;
			float cos = std::cos(rotation);
			float sin = std::sin(rotation);

			x = tx * cos - ty * sin + center.x;
			y = tx * sin + ty * cos + center.y;
		}

		return glm::vec2(x, y);
	}

private:
	glm::vec2 center;
	float xRadius, yRadius;
	float startAngle, endAngle, delta;
	bool clockwise;
	float rotation;
};
