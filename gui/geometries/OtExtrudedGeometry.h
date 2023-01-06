//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtGeometry.h"
#include "OtShape.h"


//
//	OtExtrudedGeometryClass
//

class OtExtrudedGeometryClass;
typedef std::shared_ptr<OtExtrudedGeometryClass> OtExtrudedGeometry;

class OtExtrudedGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setShape(OtObject shape);
	OtObject setDepth(float depth);
	OtObject setSegments(int segments);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtExtrudedGeometry create();

private:
	// generate geometry
	void fillGeometry() override;

	// attributes
	OtShape shape;
	float depth = 1.0;
	int segments = 12;
};
