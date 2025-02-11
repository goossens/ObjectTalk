//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCurveSegment.h"


//
//	OtCubicBezierSegment
//

class OtCubicBezierSegment : public OtCurveSegment {
public:
	// constructor
	inline OtCubicBezierSegment(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4) {
		p1 = v1;
		p2 = v2;
		p3 = v3;
		p4 = v4;
	}

	// get a point in the segment at curve parameter t [0, 1]
	glm::vec2 getPoint(float t) override {
		return glm::vec2(cubicBezier(t, p1.x, p2.x, p3.x, p4.x), cubicBezier(t, p1.y, p2.y, p3.y, p4.y));
	}

	// convert segment to string representation
	inline std::string toString() override {
		return fmt::format("C {} {} {} {} {} {}\n", p2.x, p2.y, p3.x, p3.y, p4.x, p4.y);
	}

private:
	// control points
	glm::vec2 p1, p2, p3, p4;

	// get approximate length of curve
	float getLength() override {
		return (glm::distance(p1, p4) * 2 + glm::distance(p1, p2) + glm::distance(p2, p3) + glm::distance(p3, p4)) / 3.0f;
	}

	// get value at curve parameter t [0, 1]
	static inline float cubicBezier(float t, float v1, float v2, float v3, float v4) {
		float k = 1 - t;

		return
			(k * k * k * v1) +
			(3.0f * k * k * t * v2) +
			(3.0f * k * t * t * v3) +
			(t * t * t * v4);
	}
};
