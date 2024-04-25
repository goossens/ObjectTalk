//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <vector>

#include "fmt/core.h"
#include "glm/glm.hpp"

#include "OtShapeSegment.h"
#include "OtCubicBezierSegment.h"
#include "OtLineSegment.h"
#include "OtQuadraticBezierSegment.h"


//
//	OtShapePath
//

class OtShapePath : public OtShapeSegment {
public:
	// constructor
	inline OtShapePath() {
		lengths.push_back(0.0f);
	}

	// clear the path
	inline void clear() {
		segments.clear();
		lengths.clear();
		lengths.push_back(0.0f);
	}

	// create a path out of segments
	inline void moveTo(const glm::vec2& point) {
		startPoint = point;
		currentPoint = point;
	}

	inline void lineTo(const glm::vec2& point){
		add(std::make_shared<OtLineSegment>(currentPoint, point));
		currentPoint = point;
	}

	inline void quadraticCurveTo(const glm::vec2& control, const glm::vec2& point) {
		add(std::make_shared<OtQuadraticBezierSegment>(currentPoint, control, point));
		currentPoint = point;
	}

	inline void bezierCurveTo(const glm::vec2& control1, const glm::vec2& control2, const glm::vec2& point) {
		add(std::make_shared<OtCubicBezierSegment>(currentPoint, control1, control2, point));
		currentPoint = point;
	}

	inline void close() {
		if (startPoint != currentPoint) {
			moveTo(startPoint);
		}
	}

	// get a point on the path at t [0, 1]
	inline glm::vec2 getPoint(float t) override {
		return getPointAtArcLength(t);
	}

	// get a point on the path at arc length u [0, 1]
	inline glm::vec2 getPointAtArcLength(float u) override {
		float fraction;
		auto segment = findSegment(u, &fraction);
		return segment->getPointAtArcLength(fraction);
	}

	// get a unit vector tangent
	inline glm::vec2 getTangent(float t) override {
		return getTangentAt(t);
	}

	inline glm::vec2 getTangentAt(float u) override {
		float fraction;
		auto segment = findSegment(u, &fraction);
		return segment->getTangentAt(fraction);
	}

	// get the length of the segment
	inline float getLength() override {
		return currentLength;
	}

	inline bool hasSegments() {
		return segments.size();
	}

	std::string toString() override {
		std::string result = fmt::format("m {} {}\n", startPoint.x, startPoint.y);

		for (auto& segment : segments) {
			result += segment->toString();
		}

		result += "z\n";
		return result;
	}

private:
	// add a segment
	inline void add(std::shared_ptr<OtShapeSegment> segment) {
		currentLength += segment->getLength();
		segments.push_back(segment);
		lengths.push_back(currentLength);
	}

	// find the segment at u [0, 1]
	inline std::shared_ptr<OtShapeSegment> findSegment(float u, float* fraction) {
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
					return segments[i - 1];

				} else {
					*fraction = 0.0;
					return segments[i];
				}
			}
		}

		*fraction = (target - lengths[high]) / (lengths[high + 1] - lengths[high]);
		return segments[high];
	}

	// segments that make up this path
	std::vector<std::shared_ptr<OtShapeSegment>> segments;
	glm::vec2 startPoint = glm::vec2(0.0);
	glm::vec2 currentPoint = glm::vec2(0.0);
	float currentLength = 0.0;
};
