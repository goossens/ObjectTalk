//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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

#include "OtSphereGeometry.h"


//
//	OtSphereGeometryClass::init
//

void OtSphereGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 7:
				setThetaLength(parameters[6]->operator float());

			case 6:
				setThetaStart(parameters[5]->operator float());

			case 5:
				setPhiLength(parameters[4]->operator float());

			case 4:
				setPhiStart(parameters[3]->operator float());

			case 3:
				setThetaSegments(parameters[2]->operator int());

			case 2:
				setPhiSegments(parameters[1]->operator int());

			case 1:
				setRadius(parameters[0]->operator float());
				break;

			default:
				OtExcept("Too many parameters [%ld] for [SphereGeometry] constructor (max 7)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtSphereGeometryClass::setRadius
//

OtObject OtSphereGeometryClass::setRadius(float r) {
	radius = r;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::setthetaSegments
//

OtObject OtSphereGeometryClass::setPhiSegments(int segments) {
	phiSegments = segments;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::setPhiStart
//

OtObject OtSphereGeometryClass::setPhiStart(float ps) {
	phiStart = ps;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::setPhiLength
//

OtObject OtSphereGeometryClass::setPhiLength(float pl) {
	phiLength = pl;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::setThetaStart
//

OtObject OtSphereGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::setThetaSegments
//

OtObject OtSphereGeometryClass::setThetaSegments(int segments) {
	thetaSegments = segments;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::setThetaLength
//

OtObject OtSphereGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtSphereGeometryClass::fillGeometry
//

void OtSphereGeometryClass::fillGeometry() {
	// get increments
	float ringDelta = thetaLength / phiSegments;
	float segDelta = phiLength / thetaSegments;

	// address each ring
	for (auto ring = 0; ring <= phiSegments; ring++) {
		auto theta = thetaStart + ring * ringDelta;

		// address each segment
		for (auto seg = 0; seg <= thetaSegments; seg++) {
			auto phi = phiStart + seg * segDelta;
			glm::vec2 uv = glm::vec2((float) seg / (float) thetaSegments, (float) ring / (float) phiSegments);
			auto r0 = radius * std::sin(glm::radians(theta));
			auto y0 = radius * std::cos(glm::radians(theta));
			auto x0 = r0 * -std::sin(glm::radians(phi));
			auto z0 = r0 * -std::cos(glm::radians(phi));

			// add vertex
			addVertex(OtVertex(
				glm::vec3(x0, y0, z0),
				glm::normalize(glm::vec3(x0, y0, z0)),
				uv));
		}
	}

	// add triangles and lines
	for (auto ring = 0; ring < phiSegments; ring++) {
		for (auto seg = 0; seg < thetaSegments; seg++) {
			auto a = ring * (thetaSegments + 1) + seg;
			auto b = a + (thetaSegments + 1);
			auto c = b + 1;
			auto d = a + 1;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (ring == 0) {
				addLine(a, d);
			}

			addLine(a, b);
			addLine(b, c);
		}
	}
}


//
//	OtSphereGeometryClass::renderGUI
//

bool OtSphereGeometryClass::renderGUI() {
	bool changed = false;
	changed |= ImGui::SliderInt("Phi Segments", &phiSegments, 1, 200);
	changed |= ImGui::SliderFloat("Phi Start", &phiStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Phi Length", &phiLength, 0.0f, 360.0f);
	changed |= ImGui::SliderInt("Theta Segments", &thetaSegments, 1, 100);
	changed |= ImGui::SliderFloat("Theta Start", &thetaStart, 0.0f, 180.0f);
	changed |= ImGui::SliderFloat("Theta Length", &thetaLength, 0.0f, 180.0f);
	changed |= OtGeometryClass::renderGUI();

	if (changed) {
		refreshGeometry = true;
	}

	return changed;
}


//
//	OtSphereGeometryClass::serialize
//

nlohmann::json OtSphereGeometryClass::serialize(std::filesystem::path* basedir) {
	auto data = OtGeometryClass::serialize(basedir);
	data["type"] = name;
	data["phiSegments"] = phiSegments;
	data["phiStart"] = phiStart;
	data["phiLength"] = phiLength;
	data["thetaSegments"] = thetaSegments;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtSphereGeometryClass::deserialize
//

void OtSphereGeometryClass::deserialize(nlohmann::json data, std::filesystem::path* basedir) {
	OtGeometryClass::deserialize(data, basedir);
	phiSegments = data.value("phiSegments", 32);
	phiStart = data.value("phiStart", 0.0f);
	phiLength = data.value("phiLength", 360.0f);
	thetaSegments = data.value("thetaSegments", 64);
	thetaStart = data.value("thetaStart", 0.0f);
	thetaLength = data.value("thetaLength", 180.0f);
	refreshGeometry = true;
}


//
//	OtSphereGeometryClass::getMeta
//

OtType OtSphereGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtSphereGeometryClass>("SphereGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtSphereGeometryClass::init));
		type->set("setRadius", OtFunction::create(&OtSphereGeometryClass::setRadius));
		type->set("setPhiSegments", OtFunction::create(&OtSphereGeometryClass::setPhiSegments));
		type->set("setPhiStart", OtFunction::create(&OtSphereGeometryClass::setPhiStart));
		type->set("setPhiLength", OtFunction::create(&OtSphereGeometryClass::setPhiLength));
		type->set("setThetaSegments", OtFunction::create(&OtSphereGeometryClass::setThetaSegments));
		type->set("setThetaStart", OtFunction::create(&OtSphereGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunction::create(&OtSphereGeometryClass::setThetaLength));
	}

	return type;
}
