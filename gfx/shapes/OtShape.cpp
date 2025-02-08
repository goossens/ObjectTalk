//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>
#include <sstream>

#include "OtFunction.h"

#include "OtShape.h"
#include "OtFont.h"


//
//	OtShape::load
//

void OtShape::load(const std::string& path) {
	clear();

	// read shape from file
	try {
		std::ifstream stream(path.c_str());

		if (stream.fail()) {
			OtError("Can't open file [{}] for reading", path);
		}

		std::string line;

		while (std::getline(stream, line)) {
		    std::istringstream iss(line);
			char command;

			if (line[0] == 'm') {
				float x, y;
				iss >> command >> x >> y;
				moveTo(x, y);

			} else if (line[0] == 'l') {
				float x, y;
				iss >> command >> x >> y;
				lineTo(x, y);

			} else if (line[0] == 'q') {
				float cx, cy, x, y;
				iss >> command >> cx >> cy >> x >> y;
				quadraticCurveTo(cx, cy, x, y);

			} else if (line[0] == 'c') {
				float cx1, cy1, cx2, cy2, x, y;
				iss >> command >> cx1 >> cy1 >> cx2 >> cy2 >> x >> y;
				bezierCurveTo(cx1, cy1, cx2, cy2, x, y);

			} else if (line[0] == 'z') {
				close();
			}
		}

		stream.close();

	} catch (std::exception& e) {
		OtError("Can't read from file [{}], error: {}", path, e.what());
	}
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

void OtShape::moveTo(float x, float y) {
	if (currentPath.hasSegments()) {
		close();
	}

	currentPath.moveTo(glm::vec2(x, y));
}


//
//	OtShape::lineTo
//

void OtShape::lineTo(float x, float y) {
	currentPath.lineTo(glm::vec2(x, y));
}


//
//	OtShape::quadraticCurveTo
//

void OtShape::quadraticCurveTo(float cx, float cy, float x, float y) {
	currentPath.quadraticCurveTo(glm::vec2(cx, cy), glm::vec2(x, y));
}


//
//	OtShape::quadBezierTo
//

void OtShape::bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {
	currentPath.bezierCurveTo(glm::vec2(cx1, cy1), glm::vec2(cx2, cy2), glm::vec2(x, y));
}


//
//	OtShape::close
//

void OtShape::close() {
	if (currentPath.hasSegments()) {
		currentPath.close();

		if (!paths) {
			paths = std::make_shared<std::vector<OtShapePath>>();
		}

		paths->push_back(currentPath);
		currentPath.clear();
		incrementVersion();
	}
}


//
//	OtShape::circle
//

void OtShape::circle(float x, float y, float radius) {
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
}


//
//	OtShape::text
//

void OtShape::text(OtFont& font, const std::string& text, float size, bool center) {
	close();

	auto width = font.getWidth(text, size);
	auto dx = center ? -width / 2.0f : 0.0f;

	font.parseGlyph(
		text, size,
		[&](float x, float y) { moveTo(dx + x, y); },
		[&](float x, float y) { lineTo(dx + x, y); },
		[&](float cx, float cy, float x, float y) { quadraticCurveTo(dx + cx, cy, dx + x, y); },
		[&](float cx1, float cy1, float cx2, float cy2, float x, float y) { bezierCurveTo(dx + cx1, cy1, dx + cx2, cy2, dx + x, y); });

	close();
}
