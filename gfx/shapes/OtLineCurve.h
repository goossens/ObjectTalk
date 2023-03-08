//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	glm::vec2 getPoint(float t) override {
		return glm::mix(p1, p2, t);
	}

	// get a point on the curve at distance n [0, 1]
	glm::vec2 getPointAt(float u) override {
		return getPoint(u);
	}

	// get a unit vector tangent at t  [0, 1]
	glm::vec2 getTangent(float t) override {
		return glm::normalize(p2 - p1);
	}

	virtual glm::vec2 getTangentAt(float u) override {
		return getTangent(u);
	}

	// get the length of the curve
	float getLength() override {
		return glm::distance(p1, p2);
	}

private:
	glm::vec2 p1, p2;
};
