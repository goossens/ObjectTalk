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
//	OtBoxClass
//

class OtBoxClass;
typedef std::shared_ptr<OtBoxClass> OtBox;

class OtBoxClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidth(double width);
	OtObject setHeight(double height);
	OtObject setDepth(double depth);
	OtObject setWidthSegments(int widthSegments);
	OtObject setHeightSegments(int heightSegments);
	OtObject setDepthSegments(int depthSegments);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtBox create();

private:
	// generate geometry
	void fillBuffers();

	// create one side of the box
	void buildPlane(int udir, int vdir, double w, double h, double d, int gridX, int gridY, glm::vec3 (*cb)(float, float, float));

	// attributes
	double width = 1.0;
	double height = 1.0;
	double depth = 1.0;
	int widthSegments = 1;
	int heightSegments = 1;
	int depthSegments = 1;
};
