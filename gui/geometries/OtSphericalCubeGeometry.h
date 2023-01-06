//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtNumbers.h"

#include "OtPolyhedronGeometry.h"


//
//	OtSphericalCubeGeometryClass
//

class OtSphericalCubeGeometryClass;
typedef std::shared_ptr<OtSphericalCubeGeometryClass> OtSphericalCubeGeometry;

class OtSphericalCubeGeometryClass : public OtPolyhedronGeometryClass {
public:
	// constructor
	OtSphericalCubeGeometryClass();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtSphericalCubeGeometry create();

private:
};
