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
//	OtDodecahedronGeometryClass
//

class OtDodecahedronGeometryClass;
typedef std::shared_ptr<OtDodecahedronGeometryClass> OtDodecahedronGeometry;

class OtDodecahedronGeometryClass : public OtPolyhedronGeometryClass {
public:
	// constructor
	OtDodecahedronGeometryClass();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtDodecahedronGeometry create();

private:
};
