//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFactory.h"

#include "OtPrimitiveBase.h"

#include "OtCubePrimitive.h"
#include "OtCirclePrimitive.h"
#include "OtCylinderPrimitive.h"
#include "OtDodecahedronPrimitive.h"
#include "OtGridPrimitive.h"
#include "OtIcosahedronPrimitive.h"
#include "OtOctahedronPrimitive.h"
#include "OtPlanePrimitive.h"
#include "OtPolyhedronPrimitive.h"
#include "OtSpherePrimitive.h"
#include "OtSphericalCubePrimitive.h"
#include "OtTetrahedronPrimitive.h"
#include "OtTorusPrimitive.h"


//
//	OtPrimitiveFactory
//

class OtPrimitiveFactory : public OtFactory<
	OtPrimitiveBase,
	OtGridPrimitive,
	OtPlanePrimitive,
	OtCirclePrimitive,
	OtCubePrimitive,
	OtCylinderPrimitive,
	OtSpherePrimitive,
	OtTorusPrimitive,
	OtTetrahedronPrimitive,
	OtOctahedronPrimitive,
	OtDodecahedronPrimitive,
	OtIcosahedronPrimitive,
	OtSphericalCubePrimitive> {};
