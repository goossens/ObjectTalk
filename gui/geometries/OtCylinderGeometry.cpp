//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtFunction.h"

#include "OtCylinderGeometry.h"


//
//	OtCylinderGeometryClass::init
//

void OtCylinderGeometryClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 8:
				thetaLength = parameters[7]->operator float();

			case 7:
				thetaStart = parameters[6]->operator float();

			case 6:
				openEnded = parameters[5]->operator bool();

			case 5:
				heightSegments = parameters[4]->operator int();

			case 4:
				radialSegments = parameters[3]->operator int();

			case 3:
				height = parameters[2]->operator float();

			case 2:
				bottomRadius = parameters[1]->operator float();

			case 1:
				topRadius = parameters[0]->operator float();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [CylinderGeometry] constructor (max 8)", count);
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
	return shared();
}


//
//	OtCylinderGeometryClass::setBottomRadius
//

OtObject OtCylinderGeometryClass::setBottomRadius(float r) {
	bottomRadius = r;
	refreshGeometry = true;
	return shared();
}


//
//	OtCylinderGeometryClass::setHeight
//

OtObject OtCylinderGeometryClass::setHeight(float h) {
	height = h;
	refreshGeometry = true;
	return shared();
}


//
//	OtCylinderGeometryClass::setRadialSegments
//

OtObject OtCylinderGeometryClass::setRadialSegments(int rs) {
	radialSegments = rs;
	refreshGeometry = true;
	return shared();
}


//
//	OtCylinderGeometryClass::setHeightSegments
//

OtObject OtCylinderGeometryClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshGeometry = true;
	return shared();
}


//
//	OtCylinderGeometryClass::setOpenEnded
//

OtObject OtCylinderGeometryClass::setOpenEnded(bool oe) {
	openEnded = oe;
	refreshGeometry = true;
	return shared();
}


//
//	OtCylinderGeometryClass::setThetaStart
//

OtObject OtCylinderGeometryClass::setThetaStart(float ts) {
	thetaStart = ts;
	refreshGeometry = true;
	return shared();
}


//
//	OtCylinderGeometryClass::setThetaLength
//

OtObject OtCylinderGeometryClass::setThetaLength(float tl) {
	thetaLength = tl;
	refreshGeometry = true;
	return shared();
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
	auto slope = ( bottomRadius - topRadius) / height;

	// generate each height segment
	for (auto y = 0; y <= heightSegments; y++) {
		auto v = (float) y / heightSegments;
		auto radius = v * (bottomRadius - topRadius) + topRadius;

		// generate each radial segment
		for (auto x = 0; x <= radialSegments; x++) {
			auto u = (float) x / radialSegments;
			auto theta = u * thetaLength + thetaStart;

			auto sinTheta = std::sin(theta);
			auto cosTheta = std::cos(theta);

			// add a new vertice
			addVertex(OtVertex(
				glm::vec3(radius * sinTheta, -v * height + height / 2.0, radius * cosTheta),
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
		glm::vec3(0, height / 2.0 * sign, 0),
		glm::vec3(0, sign, 0),
		glm::vec2(0.5, 0.5)));

	// add outside vertices
	auto offset = (uint32_t) vertices.size();

	for (auto x = 0; x <= radialSegments; x++) {
		auto u = (float) x / radialSegments;
		auto theta = u * thetaLength + thetaStart;
		auto cosTheta = std::cos(theta);
		auto sinTheta = std::sin(theta);

		// add vertex
		addVertex(OtVertex(
			glm::vec3(radius * sinTheta, height / 2.0 * sign, radius * cosTheta),
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
//	OtCylinderGeometryClass::getMeta
//

OtType OtCylinderGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtCylinderGeometryClass>("CylinderGeometry", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCylinderGeometryClass::init));
		type->set("setTopRadius", OtFunctionClass::create(&OtCylinderGeometryClass::setTopRadius));
		type->set("setBottomRadius", OtFunctionClass::create(&OtCylinderGeometryClass::setBottomRadius));
		type->set("setHeight", OtFunctionClass::create(&OtCylinderGeometryClass::setHeight));
		type->set("setRadialSegments", OtFunctionClass::create(&OtCylinderGeometryClass::setRadialSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtCylinderGeometryClass::setHeightSegments));
		type->set("setOpenEnded", OtFunctionClass::create(&OtCylinderGeometryClass::setOpenEnded));
		type->set("setThetaStart", OtFunctionClass::create(&OtCylinderGeometryClass::setThetaStart));
		type->set("setThetaLength", OtFunctionClass::create(&OtCylinderGeometryClass::setThetaLength));
	}

	return type;
}


//
//	OtCylinderGeometryClass::create
//

OtCylinderGeometry OtCylinderGeometryClass::create() {
	OtCylinderGeometry cylindergeometry = std::make_shared<OtCylinderGeometryClass>();
	cylindergeometry->setType(getMeta());
	return cylindergeometry;
}
