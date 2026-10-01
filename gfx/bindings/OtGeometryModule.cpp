//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtModule.h"
#include "OtObject.h"

#include "OtCirclePrimitive.h"
#include "OtCubePrimitive.h"
#include "OtCylinderPrimitive.h"
#include "OtDodecahedronPrimitive.h"
#include "OtGridPrimitive.h"
#include "OtIcosahedronPrimitive.h"
#include "OtOctahedronPrimitive.h"
#include "OtPlanePrimitive.h"
#include "OtSpherePrimitive.h"
#include "OtTetrahedronPrimitive.h"
#include "OtTorusPrimitive.h"

#include "OtGeometryModule.h"


//
//	OtGeometryClass::add
//

void OtGeometryClass::add(size_t count, OtObject* parameters) {
	for (size_t i = 0; i < count; i++) {
		parameters[i].expect<OtGeometryClass>("Geometry");
		geometry.add(OtGeometryObject(parameters[i])->geometry);
	}
}


//
//	OtGeometryClass::circle
//

OtObject OtGeometryClass::circle(size_t count, OtObject* parameters) {
	OtCirclePrimitive primitive;

	if (count == 3) {
		primitive.setSegments(parameters[0]->operator int());
		primitive.setThetaStart(parameters[1]->operator float());
		primitive.setThetaLength(parameters[2]->operator float());

	} else if (count == 1) {
		primitive.setSegments(parameters[0]->operator int());

	} else if (count != 0) {
		OtLogError("[geometry.circle] expects 0, 1 or 3 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::cube
//

OtObject OtGeometryClass::cube(size_t count, OtObject* parameters) {
	OtCubePrimitive primitive;

	if (count == 3) {
		primitive.setWidthSegments(parameters[0]->operator int());
		primitive.setHeightSegments(parameters[1]->operator int());
		primitive.setDepthSegments(parameters[2]->operator int());

	} else if (count == 1) {
		auto segments = parameters[0]->operator int();
		primitive.setWidthSegments(segments);
		primitive.setHeightSegments(segments);
		primitive.setDepthSegments(segments);

	} else if (count != 0) {
		OtLogError("[geometry.cube] expects 0, 1 or 3 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::cylinder
//

OtObject OtGeometryClass::cylinder(size_t count, OtObject* parameters) {
	OtCylinderPrimitive primitive;

	if (count == 7) {
		primitive.setTopRadius(parameters[0]->operator float());
		primitive.setBottomRadius(parameters[1]->operator float());
		primitive.setRadialSegments(parameters[2]->operator int());
		primitive.setHeightSegments(parameters[3]->operator int());
		primitive.setOpenEnded(parameters[4]->operator bool());
		primitive.setThetaStart(parameters[5]->operator float());
		primitive.setThetaLength(parameters[6]->operator float());

	} else if (count == 5) {
		primitive.setTopRadius(parameters[0]->operator float());
		primitive.setBottomRadius(parameters[1]->operator float());
		primitive.setRadialSegments(parameters[2]->operator int());
		primitive.setHeightSegments(parameters[3]->operator int());
		primitive.setOpenEnded(parameters[4]->operator bool());

	} else if (count == 4) {
		primitive.setTopRadius(parameters[0]->operator float());
		primitive.setBottomRadius(parameters[1]->operator float());
		primitive.setRadialSegments(parameters[2]->operator int());
		primitive.setHeightSegments(parameters[3]->operator int());

	} else if (count == 2) {
		primitive.setTopRadius(parameters[0]->operator float());
		primitive.setBottomRadius(parameters[1]->operator float());

	} else if (count != 0) {
		OtLogError("[geometry.cylinder] expects 0, 2, 4, 5 or 7 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::dodecahedron
//

OtObject OtGeometryClass::dodecahedron() {
	OtDodecahedronPrimitive primitive;
	return createGeometry(primitive);
}


//
//	OtGeometryClass::grid
//

OtObject OtGeometryClass::grid(size_t count, OtObject* parameters) {
	OtCubePrimitive primitive;

	if (count == 2) {
		primitive.setWidthSegments(parameters[0]->operator int());
		primitive.setDepthSegments(parameters[1]->operator int());

	} else if (count != 0) {
		OtLogError("[geometry.grid] expects 0 or 2 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::icosahedron
//

OtObject OtGeometryClass::icosahedron() {
	OtIcosahedronPrimitive primitive;
	return createGeometry(primitive);
}


//
//	OtGeometryClass::octahedron
//

OtObject OtGeometryClass::octahedron() {
	OtOctahedronPrimitive primitive;
	return createGeometry(primitive);
}


//
//	OtGeometryClass::plane
//

OtObject OtGeometryClass::plane(size_t count, OtObject* parameters) {
	OtPlanePrimitive primitive;

	if (count == 2) {
		primitive.setWidthSegments(parameters[0]->operator int());
		primitive.setHeightSegments(parameters[1]->operator int());

	} else if (count != 0) {
		OtLogError("[geometry.plane] expects 0 or 2 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::sphere
//

OtObject OtGeometryClass::sphere(size_t count, OtObject* parameters) {
	OtSpherePrimitive primitive;

	if (count == 7) {
		primitive.setRadius(parameters[0]->operator float());
		primitive.setPhiSegments(parameters[1]->operator int());
		primitive.setPhiStart(parameters[2]->operator float());
		primitive.setPhiLength(parameters[3]->operator float());
		primitive.setThetaSegments(parameters[4]->operator int());
		primitive.setThetaStart(parameters[5]->operator float());
		primitive.setThetaLength(parameters[6]->operator float());

	} else if (count == 1) {
		primitive.setRadius(parameters[0]->operator float());

	} else if (count != 0) {
		OtLogError("[geometry.sphere] expects 0, 1 or 7 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::tetrahedron
//

OtObject OtGeometryClass::tetrahedron() {
	OtTetrahedronPrimitive primitive;
	return createGeometry(primitive);
}


//
//	OtGeometryClass::torus
//

OtObject OtGeometryClass::torus(size_t count, OtObject* parameters) {
	OtTorusPrimitive primitive;

	if (count == 8) {
		primitive.setRadius(parameters[0]->operator float());
		primitive.setTubeRadius(parameters[1]->operator float());
		primitive.setRadialSegments(parameters[2]->operator int());
		primitive.setTubularSegments(parameters[3]->operator int());
		primitive.setRadialStart(parameters[4]->operator float());
		primitive.setRadialLength(parameters[5]->operator float());
		primitive.setTubularStart(parameters[6]->operator float());
		primitive.setTubularLength(parameters[7]->operator float());

	} else if (count == 4) {
		primitive.setRadius(parameters[0]->operator float());
		primitive.setTubeRadius(parameters[1]->operator float());
		primitive.setRadialSegments(parameters[2]->operator int());
		primitive.setTubularSegments(parameters[3]->operator int());

	} else if (count == 2) {
		primitive.setRadius(parameters[0]->operator float());
		primitive.setTubeRadius(parameters[1]->operator float());

	} else if (count != 0) {
		OtLogError("[geometry.torus] expects 0, 2, 4 or 8 arguments (not {})", count);
	}

	return createGeometry(primitive);
}


//
//	OtGeometryClass::getMeta
//

OtType OtGeometryClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtGeometryClass>("Geometry", OtObjectClass::getMeta());
		type->set("clear", OtFunction::create(&OtGeometryClass::clear));

		type->set("load", OtFunction::create(&OtGeometryClass::load));
		type->set("save", OtFunction::create(&OtGeometryClass::save));

		type->set("add", OtFunction::create(&OtGeometryClass::add));
		type->set("optimize", OtFunction::create(&OtGeometryClass::optimize));
	}

	return type;
}


//
//	Module registration
//

static OtModuleRegistration registration{"geometry", [](OtModule module) {
	// register classes
	module->set("Geometry", OtClass::create(OtGeometryClass::getMeta()));

	// register functions
	module->set("circle", OtFunction::create(OtGeometryClass::circle));
	module->set("cube", OtFunction::create(OtGeometryClass::cube));
	module->set("cylinder", OtFunction::create(OtGeometryClass::cylinder));
	module->set("dodecahedron", OtFunction::create(OtGeometryClass::dodecahedron));
	module->set("grid", OtFunction::create(OtGeometryClass::grid));
	module->set("icosahedron", OtFunction::create(OtGeometryClass::icosahedron));
	module->set("octahedron", OtFunction::create(OtGeometryClass::octahedron));
	module->set("sphere", OtFunction::create(OtGeometryClass::sphere));
	module->set("tetrahedron", OtFunction::create(OtGeometryClass::tetrahedron));
	module->set("torus", OtFunction::create(OtGeometryClass::torus));
}};
