//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtGeometry.h"


//
//	OtCustomGeometryClass
//

class OtCustomGeometryClass;
typedef std::shared_ptr<OtCustomGeometryClass> OtCustomGeometry;

class OtCustomGeometryClass : public OtGeometryClass {
public:
	OtCustomGeometryClass() {
		refreshGeometry = false;
	}

	// make parent's member function public
	using OtGeometryClass::addVertex;
	using OtGeometryClass::addTriangle;
	using OtGeometryClass::addLine;
	using OtGeometryClass::addIndex;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtCustomGeometry create();
};
