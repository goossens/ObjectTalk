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
			case 3:
				thetaLength = parameters[3]->operator float();

			case 2:
				thetaStart = parameters[2]->operator float();

			case 1:
				segments = parameters[1]->operator int();

			default:
				OtExcept("Too many parameters [%ld] for [CircleGeometry] constructor (max 3)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtCircleGeometryClass::setSegments
//

OtObject OtCircleGeometryClass::setSegments(int s) {
	segments = s;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCircleGeometryClass::setThetaStart
//

OtObject OtCircleGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCircleGeometryClass::setThetaLength
//

OtObject OtCircleGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCircleGeometryClass::fillGeometry
//

void OtCircleGeometryClass::fillGeometry() {
	// add center
	addVertex(OtVertex(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)),
		glm::vec2(0.5f, 0.5f)));

	// add points on circle
	float delta = thetaLength / segments;

	for (auto c = 0; c <= segments; c++) {
		auto angle = glm::radians(thetaStart + c * delta);
		auto x = std::cos(angle);
		auto y = std::sin(angle);

		addVertex(OtVertex(
			glm::vec3(x, y, 0),
			glm::normalize(glm::vec3(0, 0, 1)),
			glm::vec2((x + 1.0f) / 2.0f, 1.0f - ((y + 1.0f) / 2.0f))));
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
	bool changed = false;
	changed |= ImGui::SliderInt("Segments", &segments, 1, 50);
	changed |= ImGui::SliderFloat("Theta Start", &thetaStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Theta Length", &thetaLength, 0.0f, 360.0f);
	changed |= OtGeometryClass::renderGUI();

	if (changed) {
		refreshGeometry = true;
	}

	return changed;
}


//
//	OtCircleGeometryClass::serialize
//

nlohmann::json OtCircleGeometryClass::serialize() {
	auto data = nlohmann::json::object();
	data["type"] = name;
	data.update(OtGeometryClass::serialize());
	data["segments"] = segments;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtCircleGeometryClass::deserialize
//

void OtCircleGeometryClass::deserialize(nlohmann::json data) {
	OtGeometryClass::deserialize(data);
	segments = data.value("segments", 32);
	thetaStart = data.value("thetaStart", 0.0f);
	thetaLength = data.value("thetaLength", 360.0f);
	refreshGeometry = true;
}


//
//	OtCircleGeometryClass::getMeta
//

OtType OtCircleGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCircleGeometryClass>("CircleGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtCircleGeometryClass::init));
		type->set("setSegments", OtFunction::create(&OtCircleGeometryClass::setSegments));
		type->set("setThetaStart", OtFunction::create(&OtCircleGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunction::create(&OtCircleGeometryClass::setThetaLength));
	}

	return type;
}
