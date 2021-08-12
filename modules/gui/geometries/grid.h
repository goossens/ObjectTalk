//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "geometry.h"


//
//	OtGridClass
//

class OtGridClass;
typedef std::shared_ptr<OtGridClass> OtGrid;

class OtGridClass : public OtGeometryClass {
public:
	// constructor
	OtGridClass();

	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGrid create();

private:
	// create vertices
	void createTriangles();

	// attributes
	int rows = 10;
	int columns = 10;
	double size = 5.0;
};
