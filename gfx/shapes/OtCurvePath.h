//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	inline OtCurvePathClass() {
		lengths.push_back(0.0f);
	}

	// create a path out of segments
	inline void moveTo(const glm::vec2& point) {
		startPoint = point;
		currentPoint = point;
	}

	inline void lineTo(const glm::vec2& point){
		add(std::make_shared<OtLineCurveClass>(currentPoint, point));
		currentPoint = point;
	}

	inline void quadraticCurveTo(const glm::vec2& control, const glm::vec2& point) {
		add(std::make_shared<OtQuadraticBezierCurveClass>(currentPoint, control, point));
		currentPoint = point;
	}

	inline void bezierCurveTo(const glm::vec2& control1, const glm::vec2& control2, const glm::vec2& point) {
		add(std::make_shared<OtCubicBezierCurveClass>(currentPoint, control1, control2, point));
		currentPoint = point;
	}

	inline void close() {
		if (startPoint != currentPoint) {
			moveTo(startPoint);
		}
	}

	// set path to well known geometries
	inline void circle(float x, float y, float radius, bool clockwise=false) {
		add(std::make_shared<OtArcCurveClass>(glm::vec2(x, y), radius, 0.0f, (float) std::numbers::pi2, clockwise));
	}

	// get a point on the path at t [0, 1]
	inline glm::vec2 getPoint(float t) override {
		return getPointAt(t);
	}

	// get a point on the path at distance n [0, 1]
	inline glm::vec2 getPointAt(float u) override {
		float fraction;
		OtCurve curve = findSegment(u, &fraction);
		return curve->getPointAt(fraction);
	}

	// get a unit vector tangent
	inline glm::vec2 getTangent(float t) override {
		return getTangentAt(t);
	}

	inline glm::vec2 getTangentAt(float u) override {
		float fraction;
		OtCurve curve = findSegment(u, &fraction);
		return curve->getTangentAt(fraction);
	}

	// get the length of the curve
	inline float getLength() override {
		return currentLength;
	}

	inline bool hasSegments() {
		return curves.size();
	}

private:
	// add a curve segment
	inline void add(OtCurve curve) {
		curves.push_back(curve);
		currentLength += curve->getLength();
		lengths.push_back(currentLength);
	}

	// find the segment at u [0, 1]
	inline OtCurve findSegment(float u, float* fraction) {
		float target = currentLength * u;
		size_t low = 0;
		size_t high = lengths.size() - 1;
		size_t i;

		while (low <= high) {
			i = (low + high) / 2;

			if (target > lengths[i]) {
				low = i + 1;

			} else if (target < lengths[i]) {
				high = i - 1;

			} else {
				if (i == lengths.size() - 1) {
					*fraction = 1.0;
					return curves[i - 1];

				} else {
					*fraction = 0.0;
					return curves[i];
				}
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
