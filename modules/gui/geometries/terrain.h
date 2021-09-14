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
#include "heightmap.h"


//
//	OtTerrainClass
//

class OtTerrainClass;
typedef std::shared_ptr<OtTerrainClass> OtTerrain;

class OtTerrainClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setWidth(double width);
	OtObject setDepth(double depth);
	OtObject setWidthSegments(int widthSegments);
	OtObject setDepthSegments(int depthSegments);
	OtObject setHeightMap(OtObject heightmap);

	// get information
	double getWidth() { return width; }
	double getDepth() { return depth; }
	int getWidthSegments() { return widthSegments; }
	int getDepthSegments() { return depthSegments; }
	OtHeightMap getHeightMap() { return heightmap; }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTerrain create();

private:
	// generate geometry
	void fillGeometry();

	// attributes
	double width = 1.0;
	double depth = 1.0;
	int widthSegments = 1;
	int depthSegments = 1;
	OtHeightMap heightmap;
};
