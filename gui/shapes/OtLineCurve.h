//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCurve.h"


//
//	OtLineCurveClass
//

class OtLineCurveClass;
typedef std::shared_ptr<OtLineCurveClass> OtLineCurve;

class OtLineCurveClass : public OtCurveClass {
public:
	// constructor
	OtLineCurveClass(const glm::vec2& v1, const glm::vec2& v2) :
		p1(v1), p2(v2) {
	}

	// get a point on the curve at t [0, 1]
	glm::vec2 getPoint(float t) {
		return glm::mix(p1, p2, t);
	}

	// get a point on the curve at distance n [0, 1]
	glm::vec2 getPointAt(float u) {
		return getPoint(u);
	}

	// get a unit vector tangent at t  [0, 1]
	glm::vec2 getTangent(float t) {
		return glm::normalize(p2 - p1);
	}

	virtual glm::vec2 getTangentAt(float u) {
		return getTangent(u);
	}

	// get the length of the curve
	float getLength() {
		return glm::distance(p1, p2);
	}

	// create a new instance
	static OtLineCurve create(const glm::vec2& v1, const glm::vec2& v2) {
		return std::make_shared<OtLineCurveClass>(v1, v2);
	}

private:
	glm::vec2 p1, p2;
};