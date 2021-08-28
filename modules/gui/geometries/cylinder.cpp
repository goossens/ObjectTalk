//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "ot/function.h"

#include "cylinder.h"


//
//	OtCylinderClass::init
//

OtObject OtCylinderClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 8:
				thetaLength = parameters[7]->operator double();

			case 7:
				thetaStart = parameters[6]->operator double();

			case 6:
				openEnded = parameters[5]->operator bool();

			case 5:
				heightSegments = parameters[4]->operator int();

			case 4:
				radialSegments = parameters[3]->operator int();

			case 3:
				height = parameters[2]->operator double();

			case 2:
				bottomRadius = parameters[1]->operator double();
				break;

			case 1:
				topRadius = parameters[0]->operator double();
				break;

			default:
				OtExcept("Too many parameters [%ld] for [Cylinder] constructor (max 8)", count);
		}

		refreshBuffers = true;
	}

	return nullptr;
}


//
//	OtCylinderClass::setTopRadius
//

OtObject OtCylinderClass::setTopRadius(double r) {
	topRadius = r;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setBottomRadius
//

OtObject OtCylinderClass::setBottomRadius(double r) {
	bottomRadius = r;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setHeight
//

OtObject OtCylinderClass::setHeight(double h) {
	height = h;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setRadialSegments
//

OtObject OtCylinderClass::setRadialSegments(int rs) {
	radialSegments = rs;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setHeightSegments
//

OtObject OtCylinderClass::setHeightSegments(int hs) {
	heightSegments = hs;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setOpenEnded
//

OtObject OtCylinderClass::setOpenEnded(bool oe) {
	openEnded = oe;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setThetaStart
//

OtObject OtCylinderClass::setThetaStart(double ts) {
	thetaStart = ts;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::setThetaLength
//

OtObject OtCylinderClass::setThetaLength(double tl) {
	thetaLength = tl;
	refreshBuffers = true;
	return shared();
}


//
//	OtCylinderClass::fillBuffers
//

void OtCylinderClass::fillBuffers() {
	// clear geometry
	clear();

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
//	OtCylinderClass::generateTorso
//

void OtCylinderClass::generateTorso() {
	auto slope = ( bottomRadius - topRadius) / height;

	// generate each height segment
	for (auto y = 0; y <= heightSegments; y++) {
		auto v = (float) y / heightSegments;
		auto radius = v * (bottomRadius - topRadius) + topRadius;

		// generate each radial segment
		for (auto x = 0; x <= radialSegments; x++) {
			auto u = (float) x / radialSegments;
			auto theta = u * thetaLength + thetaStart;

			auto sinTheta = std::sinf(theta);
			auto cosTheta = std::cosf(theta);

			// add a new vertice
			addVertex(OtVertex(
				glm::vec3(radius * sinTheta, -v * height + height / 2.0, radius * cosTheta),
				glm::normalize(glm::vec3(sinTheta, slope, cosTheta)),
				glm::vec2(u, 1 - v)));
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
//	OtCylinderClass::generateCap
//

void OtCylinderClass::generateCap(bool top) {
	auto radius = top ? topRadius : bottomRadius;
	auto sign = top ? 1 : - 1;

	// add center
	auto center = vertices.size();

	addVertex(OtVertex(
		glm::vec3(0, height / 2.0 * sign, 0),
		glm::vec3(0, sign, 0),
		glm::vec2(0.5, 0.5)));

	// add outside vertices
	auto offset = vertices.size();

	for (auto x = 0; x <= radialSegments; x++) {
		auto u = (float) x / radialSegments;
		auto theta = u * thetaLength + thetaStart;
		auto cosTheta = std::cosf(theta);
		auto sinTheta = std::sinf(theta);

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
//	OtCylinderClass::getMeta
//

OtType OtCylinderClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCylinderClass>("Cylinder", OtGeometryClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCylinderClass::init));
		type->set("setTopRadius", OtFunctionClass::create(&OtCylinderClass::setTopRadius));
		type->set("setBottomRadius", OtFunctionClass::create(&OtCylinderClass::setBottomRadius));
		type->set("setHeight", OtFunctionClass::create(&OtCylinderClass::setHeight));
		type->set("setRadialSegments", OtFunctionClass::create(&OtCylinderClass::setRadialSegments));
		type->set("setHeightSegments", OtFunctionClass::create(&OtCylinderClass::setHeightSegments));
		type->set("setOpenEnded", OtFunctionClass::create(&OtCylinderClass::setOpenEnded));
		type->set("setThetaStart", OtFunctionClass::create(&OtCylinderClass::setThetaStart));
		type->set("setThetaLength", OtFunctionClass::create(&OtCylinderClass::setThetaLength));
	}

	return type;
}


//
//	OtCylinderClass::create
//

OtCylinder OtCylinderClass::create() {
	OtCylinder cylinder = std::make_shared<OtCylinderClass>();
	cylinder->setType(getMeta());
	return cylinder;
}
