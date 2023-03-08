//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFactory.h"
#include "OtSingleton.h"

#include "OtGeometries.h"


//
//	OtGeometryFactory
//

class OtGeometryFactory : public OtSingleton<OtGeometryFactory>, public OtFactory<
	OtGeometry,
	OtPlaneGeometryClass,
	OtCircleGeometryClass,
	OtBoxGeometryClass,
	OtCylinderGeometryClass,
	OtSphereGeometryClass,
	OtTorusGeometryClass,
	OtModelGeometryClass,
	OtTetrahedronGeometryClass,
	OtOctahedronGeometryClass,
	OtDodecahedronGeometryClass,
	OtIcosahedronGeometryClass,
	OtSphericalCubeGeometryClass> {};
