//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <cmath>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtFunction.h"

#include "OtCylinderGeometry.h"


//
//	OtCylinderGeometryClass::init
//

void OtCylinderGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 7:
				thetaLength = parameters[7]->operator float();

			case 6:
				thetaStart = parameters[6]->operator float();

			case 5:
				openEnded = parameters[5]->operator bool();

			case 4:
				heightSegments = parameters[4]->operator int();

			case 3:
				radialSegments = parameters[3]->operator int();

			case 2:
				bottomRadius = parameters[1]->operator float();

			case 1:
				topRadius = parameters[0]->operator float();
				break;

			default:
				OtError("Too many parameters [{}] for [CylinderGeometry] constructor (max 7)", count);
		}

		refreshGeometry = true;
	}
}


//
//	OtCylinderGeometryClass::setTopRadius
//

OtObject OtCylinderGeometryClass::setTopRadius(float r) {
	topRadius = r;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::setBottomRadius
//

OtObject OtCylinderGeometryClass::setBottomRadius(float r) {
	bottomRadius = r;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::setRadialSegments
//

OtObject OtCylinderGeometryClass::setRadialSegments(int rs) {
	radialSegments = rs;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::setHeightSegments
//

OtObject OtCylinderGeometryClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::setOpenEnded
//

OtObject OtCylinderGeometryClass::setOpenEnded(bool oe) {
	openEnded = oe;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::setThetaStart
//

OtObject OtCylinderGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::setThetaLength
//

OtObject OtCylinderGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return OtObject(this);
}


//
//	OtCylinderGeometryClass::fillGeometry
//

void OtCylinderGeometryClass::fillGeometry() {
	// generate a torso
	generateTorso();

	// generate end caps if required
	if (!openEnded) {
		if (topRadius > 0) {
			generateCap(true);
		}

		if (bottomRadius > 0) {
			generateCap(false);
		}
	}
}


//
//	OtCylinderGeometryClass::generateTorso
//

void OtCylinderGeometryClass::generateTorso() {
	auto slope = ( bottomRadius - topRadius);

	// generate each height segment
	for (auto y = 0; y <= heightSegments; y++) {
		auto v = (float) y / heightSegments;
		auto radius = v * (bottomRadius - topRadius) + topRadius;

		// generate each radial segment
		for (auto x = 0; x <= radialSegments; x++) {
			auto u = (float) x / radialSegments;
			auto theta = glm::radians(u * thetaLength + thetaStart);

			auto sinTheta = std::sin(theta);
			auto cosTheta = std::cos(theta);

			// add a new vertice
			addVertex(OtVertex(
				glm::vec3(radius * sinTheta, -v + 0.5f, radius * cosTheta),
				glm::normalize(glm::vec3(sinTheta, slope, cosTheta)),
				glm::vec2(u, v)));
		}
	}

	// add triangles and lines
	for (auto y = 0; y < heightSegments; y++) {
		for (auto x = 0; x < radialSegments; x++) {
			auto a = y * (radialSegments + 1) + x;
			auto d = a + 1;
			auto b = a + (radialSegments + 1);
			auto c = b + 1;

			addTriangle(a, b, d);
			addTriangle(b, c, d);

			if (y == 0) {
				addLine(a, d);
			}

			addLine(a, b);
			addLine(b, c);

			if (x == radialSegments - 1) {
				addLine(c, d);
			}
		}
	}
}


//
//	OtCylinderGeometryClass::generateCap
//

void OtCylinderGeometryClass::generateCap(bool top) {
	auto radius = top ? topRadius : bottomRadius;
	auto sign = top ? 1 : - 1;

	// add center
	auto center = (uint32_t) vertices.size();

	addVertex(OtVertex(
		glm::vec3(0.0f, 0.5f * sign, 0.0f),
		glm::vec3(0.0f, sign, 0.0f),
		glm::vec2(0.5f, 0.5f)));

	// add outside vertices
	auto offset = (uint32_t) vertices.size();

	for (auto x = 0; x <= radialSegments; x++) {
		auto u = (float) x / radialSegments;
		auto theta = glm::radians(u * thetaLength + thetaStart);
		auto cosTheta = std::cos(theta);
		auto sinTheta = std::sin(theta);

		// add vertex
		addVertex(OtVertex(
			glm::vec3(radius * sinTheta, 0.5f * sign, radius * cosTheta),
			glm::vec3(0, sign, 0),
			glm::vec2((cosTheta * 0.5) + 0.5, (sinTheta * 0.5 * sign) + 0.5)));
	}

	// add triangles and lines
	for (auto c = 0; c < radialSegments; c++) {
		if (top) {
			addTriangle(offset + c, offset + c + 1, center);

		} else {
			addTriangle(offset + c + 1, offset + c, center);
		}

		addLine(center, offset + c);
		addLine(offset + c, offset + c + 1);

		if (c == radialSegments - 1) {
			addLine(center, offset + c + 1);
		}
	}
}


//
//	OtCylinderGeometryClass::renderUI
//

bool OtCylinderGeometryClass::renderUI() {
	bool changed = false;
	changed |= ImGui::SliderFloat("Top Radius", &topRadius, 0.0f, 1.0f);
	changed |= ImGui::SliderFloat("Bottom Radius", &bottomRadius, 0.0f, 1.0f);
	changed |= ImGui::SliderInt("Radial Segments", &radialSegments, 1, 64);
	changed |= ImGui::SliderInt("Height Segments", &heightSegments, 1, 32);
	changed |= ImGui::Checkbox("Open Ended", &openEnded);
	changed |= ImGui::SliderFloat("Theta Start", &thetaStart, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat("Theta Length", &thetaLength, 0.0f, 360.0f);
	changed |= OtGeometryClass::renderUI();

	if (changed) {
		refreshGeometry = true;
	}

	return changed;
}


//
//	OtCylinderGeometryClass::serialize
//

nlohmann::json OtCylinderGeometryClass::serialize(std::string* basedir) {
	auto data = OtGeometryClass::serialize(basedir);
	data["type"] = name;
	data["topRadius"] = topRadius;
	data["bottomRadius"] = bottomRadius;
	data["radialSegments"] = radialSegments;
	data["heightSegments"] = heightSegments;
	data["openEnded"] = openEnded;
	data["thetaStart"] = thetaStart;
	data["thetaLength"] = thetaLength;
	return data;
}


//
//	OtCylinderGeometryClass::deserialize
//

void OtCylinderGeometryClass::deserialize(nlohmann::json data, std::string* basedir) {
	OtGeometryClass::deserialize(data, basedir);
	topRadius = data.value("topRadius", 1.0f);
	bottomRadius = data.value("bottomRadius", 1.0f);
	radialSegments = data.value("radialSegments", 32);
	heightSegments = data.value("heightSegments", 1);
	openEnded = data.value("openEnded", false);
	thetaStart = data.value("thetaStart", 0.0f);
	thetaLength = data.value("thetaLength", 360.0f);
	refreshGeometry = true;
}


//
//	OtCylinderGeometryClass::getMeta
//

OtType OtCylinderGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtCylinderGeometryClass>("CylinderGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunction::create(&OtCylinderGeometryClass::init));
		type->set("setTopRadius", OtFunction::create(&OtCylinderGeometryClass::setTopRadius));
		type->set("setBottomRadius", OtFunction::create(&OtCylinderGeometryClass::setBottomRadius));
		type->set("setRadialSegments", OtFunction::create(&OtCylinderGeometryClass::setRadialSegments));
		type->set("setHeightSegments", OtFunction::create(&OtCylinderGeometryClass::setHeightSegments));
		type->set("setOpenEnded", OtFunction::create(&OtCylinderGeometryClass::setOpenEnded));
		type->set("setThetaStart", OtFunction::create(&OtCylinderGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunction::create(&OtCylinderGeometryClass::setThetaLength));
	}

	return type;
}
