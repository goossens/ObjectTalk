//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"

#include "OtNumbers.h"

#include "OtArcCurve.h"
#include "OtCurve.h"
#include "OtCubicBezierCurve.h"
#include "OtLineCurve.h"
#include "OtQuadraticBezierCurve.h"


//
//	OtCurvePathClass
//

class OtCurvePathClass;
typedef std::shared_ptr<OtCurvePathClass> OtCurvePath;

class OtCurvePathClass : public OtCurveClass {
public:
	// constructor
	OtCurvePathClass() {
		lengths.push_back(0.0);
	}

	// create a path out of segments
	void moveTo(const glm::vec2& point) {
		startPoint = point;
		currentPoint = point;
	}

	void lineTo(const glm::vec2& point){
		add(OtLineCurveClass::create(currentPoint, point));
		currentPoint = point;
	}

	void quadraticCurveTo(const glm::vec2& control, const glm::vec2& point) {
		add(OtQuadraticBezierCurveClass::create(currentPoint, control, point));
		currentPoint = point;
	}

	void bezierCurveTo(const glm::vec2& control1, const glm::vec2& control2, const glm::vec2& point) {
		add(OtCubicBezierCurveClass::create(currentPoint, control1, control2, point));
		currentPoint = point;
	}

	void close() {
		if (startPoint != currentPoint) {
			moveTo(startPoint);
		}
	}

	// set path to well known geometries
	void circle(float x, float y, float radius, bool clockwise=false) {
		add(OtArcCurveClass::create(glm::vec2(x, y), radius, 0.0, std::numbers::pi2, clockwise));
	}

	// get a point on the path at t [0, 1]
	glm::vec2 getPoint(float t) {
		return getPointAt(t);
	}

	// get a point on the path at distance n [0, 1]
	glm::vec2 getPointAt(float u) {
		float fraction;
		OtCurve curve = findSegment(u, &fraction);
		return curve->getPointAt(fraction);
	}

	// get a unit vector tangent
	glm::vec2 getTangent(float t) {
		return getTangentAt(t);
	}

	glm::vec2 getTangentAt(float u) {
		float fraction;
		OtCurve curve = findSegment(u, &fraction);
		return curve->getTangentAt(fraction);
	}

	// get the length of the curve
	float getLength() {
		return currentLength;
	}

	bool hasSegments() {
		return curves.size();
	}

	// create a new instance
	static OtCurvePath create() {
		return std::make_shared<OtCurvePathClass>();
	}

private:
	// add a curve segment
	void add(OtCurve curve) {
		curves.push_back(curve);
		currentLength += curve->getLength();
		lengths.push_back(currentLength);
	}

	// find the segment at u [0, 1]
	OtCurve findSegment(float u, float* fraction) {
		float target = currentLength * u;
		int low = 0;
		int high = lengths.size() - 1;
		int i;

		while (low <= high) {
			i = (low + high) / 2;

			if (target > lengths[i]) {
				low = i + 1;

			} else if (target < lengths[i]) {
				high = i - 1;

			} else {
				*fraction = 0.0;
				return curves[i];
			}
		}

		*fraction = (target - lengths[high]) / (lengths[high + 1] - lengths[high]);
		return curves[high];
	}

	// curves that make up this path
	std::vector<OtCurve> curves;
	glm::vec2 startPoint = glm::vec2(0.0);
	glm::vec2 currentPoint = glm::vec2(0.0);
	float currentLength = 0.0;
};
