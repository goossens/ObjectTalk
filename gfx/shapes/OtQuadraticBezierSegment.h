//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtShapeSegment.h"


//
//	OtQuadraticBezierSegment
//

class OtQuadraticBezierSegment : public OtShapeSegment {
public:
	// constructor
	inline OtQuadraticBezierSegment(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3) {
		p1 = v1;
		p2 = v2;
		p3 = v3;
	}

	// get a point in the segment at curve parameter t [0, 1]
	inline glm::vec2 getPoint(float t) override {
		return glm::vec2(quatraticBezier(t, p1.x, p2.x, p3.x), quatraticBezier(t, p1.y, p2.y, p3.y));
	}

	// convert segment to string representation
	inline std::string toString() override {
		return fmt::format("q {} {} {} {}\n", p2.x, p2.y, p3.x, p3.y);
	}

private:
	// control points
	glm::vec2 p1, p2, p3;

	static inline float quatraticBezier(float t, float v1, float v2, float v3) {
		float k = 1.0f - t;

		return (k * k * v1) +
			(2.0f * k * t * v2) +
			(t * t * v3);
	}
};
