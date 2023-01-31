//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "glm/glm.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtCircleGeometry.h"


//
//	OtCircleGeometryClass::init
//

void OtCircleGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 4:
				thetaLength = parameters[3]->operator float();

			case 3:
				thetaStart = parameters[2]->operator float();

			case 2:
				segments = parameters[1]->operator int();

			case 1:
				radius = parameters[0]->operator float();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [CircleGeometry] constructor (max 4)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtCircleGeometryClass::setRadius
//

OtObject OtCircleGeometryClass::setRadius(float r) {
	radius = r;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleGeometryClass::setSegments
//

OtObject OtCircleGeometryClass::setSegments(int s) {
	segments = s;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleGeometryClass::setThetaStart
//

OtObject OtCircleGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleGeometryClass::setThetaLength
//

OtObject OtCircleGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return shared();
}


//
//	OtCircleGeometryClass::fillGeometry
//

void OtCircleGeometryClass::fillGeometry() {
	// add center
	addVertex(OtVertex(
		glm::vec3(0, 0, 0),
		glm::normalize(glm::vec3(0, 0, 1)),
		glm::vec2(0.5, 0.5)));

	// add points on circle
	float delta = thetaLength / segments;

	for (auto c = 0; c <= segments; c++) {
		auto angle = thetaStart + c * delta;
		auto x = std::cos(angle);
		auto y = std::sin(angle);

		addVertex(OtVertex(
			glm::vec3(radius * x, radius * y, 0),
			glm::normalize(glm::vec3(0, 0, 1)),
			glm::vec2((x + 1.0) / 2.0, 1.0 - ((y + 1.0) / 2.0))));
	}

	// add triangles and lines
	for (auto c = 1; c <= segments; c++) {
		addTriangle(c, c + 1, 0);

		addLine(0, c);
		addLine(c, c + 1);

		if (c == segments) {
			addLine(0, c + 1);
		}
	}
}


//
//	OtCircleGeometryClass::renderGUI
//

bool OtCircleGeometryClass::renderGUI() {
	bool changed = OtGeometryClass::renderGUI();
	changed |= ImGui::SliderInt("Segments", &segments, 1, 50);

	int start = glm::degrees(thetaStart);

	if (ImGui::SliderInt("Theta Start", &start, 0, 359)) {
		thetaStart = glm::radians((float) start);
		changed |= true;
	}

	int length = glm::degrees(thetaLength);

	if (ImGui::SliderInt("Theta Length", &length, 1, 360)) {
		thetaLength = glm::radians((float) length);
		changed |= true;
	}

	return changed;
}


//
//	OtCircleGeometryClass::serialize
//

nlohmann::json OtCircleGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	data["segments"] = segments;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtCircleGeometryClass::deserialize
//

void OtCircleGeometryClass::deserialize(nlohmann::json data) {
	segments = data["segments"];
	thetaStart = data["thetaStart"];
	thetaLength = data["thetaLength"];
}


//
//	OtCircleGeometryClass::getMeta
//

OtType OtCircleGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtCircleGeometryClass>("CircleGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCircleGeometryClass::init));
		type->set("setRadius", OtFunctionClass::create(&OtCircleGeometryClass::setRadius));
		type->set("setSegments", OtFunctionClass::create(&OtCircleGeometryClass::setSegments));
		type->set("setThetaStart", OtFunctionClass::create(&OtCircleGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunctionClass::create(&OtCircleGeometryClass::setThetaLength));
	}

	return type;
}


//
//	OtCircleGeometryClass::create
//

OtCircleGeometry OtCircleGeometryClass::create() {
	OtCircleGeometry circlegeometry = std::make_shared<OtCircleGeometryClass>();
	circlegeometry->setType(getMeta());
	return circlegeometry;
}
