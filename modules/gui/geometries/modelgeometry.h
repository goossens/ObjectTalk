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
//	OtModelGeometryClass
//

class OtModelGeometryClass;
typedef std::shared_ptr<OtModelGeometryClass> OtModelGeometry;

class OtModelGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setModel(const std::string& name);
	OtObject setScale(float scale);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtModelGeometry create();

private:
	// generate geometry
	void fillGeometry();

	// properties
	std::string modelName;
	float scale = 1.0;
};
