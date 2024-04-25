//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "OtFunction.h"

#include "OtShape.h"
#include "OtFont.h"


//
//	OtShape::load
//

void OtShape::load(const std::string &path) {
}


//
//	OtShape::save
//

void OtShape::save(const std::string& path) {
	std::string output;

	// see if we have any paths
	if (paths) {
		for (auto& path : *paths) {
			output += path.toString();
		}
	}

	// write shape to file
	try {
		std::ofstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't open file [{}] for writing", path);
		}

		stream << output;
		stream.close();

	} catch (std::exception& e) {
		OtError("Can't write to file [{}], error: {}", path, e.what());
	}
}


//
//	OtShape::moveTo
//

OtShape* OtShape::moveTo(float x, float y) {
	if (currentPath.hasSegments()) {
		close();
	}

	currentPath.moveTo(glm::vec2(x, y));
	return this;
}


//
//	OtShape::lineTo
//

OtShape* OtShape::lineTo(float x, float y) {
	currentPath.lineTo(glm::vec2(x, y));
	return this;
}


//
//	OtShape::quadraticCurveTo
//

OtShape* OtShape::quadraticCurveTo(float cx, float cy, float x, float y) {
	currentPath.quadraticCurveTo(glm::vec2(cx, cy), glm::vec2(x, y));
	return this;
}


//
//	OtShape::quadBezierTo
//

OtShape* OtShape::bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {
	currentPath.bezierCurveTo(glm::vec2(cx1, cy1), glm::vec2(cx2, cy2), glm::vec2(x, y));
	return this;
}


//
//	OtShape::close
//

OtShape* OtShape::close() {
	if (currentPath.hasSegments()) {
		currentPath.close();

		if (!paths) {
			paths = std::make_shared<std::vector<OtShapePath>>();
		}

		paths->push_back(currentPath);
		currentPath.clear();
		incrementVersion();
	}

	return this;
}


//
//	OtShape::circle
//

OtShape* OtShape::circle(float x, float y, float radius) {
	// see https://stackoverflow.com/questions/1734745/how-to-create-circle-with-bézier-curves
	// for explanation of this approximation
	auto segment = [&](float x, float y, float rx, float ry) {
		moveTo(x - rx, y);
		bezierCurveTo(x - rx, y - 0.552 * ry, x - 0.552 * rx, y - ry,  x,  y - ry);
	};

	close();
	segment(x, y, -radius, radius);
	segment(x, y, radius, radius);
	segment(x, y, radius, -radius);
	segment(x, y, -radius, -radius);
	close();

	return this;
}


//
//	OtShape::text
//

OtShape* OtShape::text(OtFont& font, const std::string& text, float size) {
	close();

	font.parseGlyph(
		text, size,
		[&](float x, float y) { moveTo(x, y); },
		[&](float x, float y) { lineTo(x, y); },
		[&](float cx, float cy, float x, float y) { quadraticCurveTo(cx, cy, x, y); },
		[&](float cx1, float cy1, float cx2, float cy2, float x, float y) { bezierCurveTo(cx1, cy1, cx2, cy2, x, y); });

	close();
	return this;
}
