//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
//	OtIcosahedronGeometryClass
//

class OtIcosahedronGeometryClass;
typedef std::shared_ptr<OtIcosahedronGeometryClass> OtIcosahedronGeometry;

class OtIcosahedronGeometryClass : public OtPolyhedronGeometryClass {
public:
	// constructor
	OtIcosahedronGeometryClass();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtIcosahedronGeometry create();

private:
};
