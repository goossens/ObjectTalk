//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtTorusGeometry.h"


//
//	OtTorusGeometryClass::init
//

void OtTorusGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 8:
				tubularLength = parameters[7]->operator float();

			case 7:
				tubularStart = parameters[6]->operator float();

			case 6:
				radialLength = parameters[5]->operator float();

			case 5:
				radialStart = parameters[4]->operator float();

			case 4:
				tubularSegments = parameters[3]->operator int();

			case 3:
				radialSegments = parameters[2]->operator int();

			case 2:
				tubeRadius = parameters[1]->operator float();

			case 1:
				radius = parameters[0]->operator float();
				break;

			default:
				OtError("Too many parameters [%ld] for [TorusGeometry] constructor (max 8)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtTorusGeometryClass::setRadius
//

OtObject OtTorusGeometryClass::setRadius(float r) {
	radius = r;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setTubeRadius
//

OtObject OtTorusGeometryClass::setTubeRadius(float tr) {
	tubeRadius = tr;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setRadialSegments
//

OtObject OtTorusGeometryClass::setRadialSegments(int segments) {
	radialSegments = segments;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setTubularSegments
//

OtObject OtTorusGeometryClass::setTubularSegments(int segments) {
	tubularSegments = segments;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setRadialStart
//

OtObject OtTorusGeometryClass::setRadialStart(float rs) {
	radialStart = rs;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setRadialLength
//

OtObject OtTorusGeometryClass::setRadialLength(float rl) {
	radialLength = rl;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setTubularStart
//

OtObject OtTorusGeometryClass::setTubularStart(float ts) {
	tubularStart = ts;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::setTubularLength
//

OtObject OtTorusGeometryClass::setTubularLength(float tl) {
	tubularLength = tl;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtTorusGeometryClass::fillGeometry
//

void OtTorusGeometryClass::fillGeometry() {
	// generate vertices
	for (auto j = 0; j <= radialSegments; j++) {
		for (auto i = 0; i <= tubularSegments; i++) {
			auto u = glm::radians(radialStart + (float) j / radialSegments * radialLength);
			auto v = glm::radians(tubularStart + (float) i / tubularSegments * tubularLength);

			auto x = (radius + tubeRadius * std::cos(v)) * std::cos(u);
			auto y = (radius + tubeRadius * std::cos(v)) * std::sin(u);
			auto z = tubeRadius * std::sin(v);

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x, y, z),
				glm::normalize(glm::vec3(x, y, z) - glm::vec3(radius * std::cos(u), radius * std::sin(u), 0.0)),
				glm::vec2((float) i / (float) tubularSegments, (float) j / (float) radialSegments)));
		}
	}

	// add triangles
	for (auto j = 1; j <= radialSegments; j++) {
		for (auto i = 1; i <= tubularSegments; i++) {
			auto a = (tubularSegments + 1) * j + i - 1;
			auto b = (tubularSegments + 1) * (j - 1) + i - 1;
			auto c = (tubularSegments + 1) * (j - 1) + i;
			auto d = (tubularSegments + 1) * j + i;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (i == 1) {
				addLine(a, b);
			}

			addLine(b, c);
			addLine(c, d);
			addLine(d, a);
		}
	}
}


//
//	OtTorusGeometryClass::renderUI
//

bool OtTorusGeometryClass::renderUI() {
	bool changed = false;
	changed |= ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
	changed |= ImGui::SliderFloat("Tube Radius", &tubeRadius, 0.0f, 1.0f);
	changed |= ImGui::SliderInt("Radial Segments", &radialSegments, 1, 64);
	changed |= ImGui::SliderInt("Tubular Segments", &tubularSegments, 1, 32);
	changed |= ImGui::SliderFloat("Radial Start", &radialStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Radial Length", &radialLength, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Tubular Start", &tubularStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Tubular Length", &tubularLength, 0.0f, 360.0f);
	changed |= OtGeometryClass::renderUI();

	if (changed) {
		refreshGeometry = true;
	}

	return changed;
}


//
//	OtTorusGeometryClass::serialize
//

nlohmann::json OtTorusGeometryClass::serialize(std::filesystem::path* basedir) {
	auto data = OtGeometryClass::serialize(basedir);
	data["type"] = name;
	data["radius"] = radius;
	data["tubeRadius"] = tubeRadius;
	data["radialSegments"] = radialSegments;
	data["tubularSegments"] = tubularSegments;
	data["radialStart"] = radialStart;
	data["radialLength"] = radialLength;
	data["tubularStart"] = tubularStart;
	data["tubularLength"] = tubularLength;
	return data;
}


//
//	OtTorusGeometryClass::deserialize
//

void OtTorusGeometryClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtGeometryClass::deserialize(data, basedir);
	radius = data.value("radius", 1.0f);
	tubeRadius = data.value("tubeRadius", 0.4f);
	radialSegments = data.value("radialSegments", 64);
	tubularSegments = data.value("tubularSegments", 32);
	radialStart = data.value("radialStart", 0.0f);
	radialLength = data.value("radialLength", 360.0f);
	tubularStart = data.value("tubularStart", 0.0f);
	tubularLength = data.value("tubularLength", 360.0f);
	refreshGeometry = true;
}


//
//	OtTorusGeometryClass::getMeta
//

OtType OtTorusGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTorusGeometryClass>("TorusGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTorusGeometryClass::init));
		type->set("setRadius", OtFunction::create(&OtTorusGeometryClass::setRadius));
		type->set("setTubeRadius", OtFunction::create(&OtTorusGeometryClass::setTubeRadius));
		type->set("setRadialSegments", OtFunction::create(&OtTorusGeometryClass::setRadialSegments));
		type->set("setTubularSegments", OtFunction::create(&OtTorusGeometryClass::setTubularSegments));
		type->set("setRadialStart", OtFunction::create(&OtTorusGeometryClass::setRadialStart));
		type->set("setRadialLength", OtFunction::create(&OtTorusGeometryClass::setRadialLength));
		type->set("setTubularStart", OtFunction::create(&OtTorusGeometryClass::setTubularStart));
		type->set("setTubularLength", OtFunction::create(&OtTorusGeometryClass::setTubularLength));
	}

	return type;
}
