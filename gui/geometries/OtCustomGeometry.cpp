//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtCustomGeometry.h"


//
//	OtCustomGeometryClass::getMeta
//

OtType OtCustomGeometryClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtCustomGeometryClass>("CustomGeometry", OtGeometryClass::getMeta());
	}

	return type;
}


//
//	OtCustomGeometryClass::create
//

OtCustomGeometry OtCustomGeometryClass::create() {
	OtCustomGeometry geometry = std::make_shared<OtCustomGeometryClass>();
	geometry->setType(getMeta());
	return geometry;
}
