//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "glm/glm.hpp"

#include "OtGui.h"
#include "OtCurvePath.h"


//
//	OtShape
//

class OtShapeClass;
typedef std::shared_ptr<OtShapeClass> OtShape;

class OtShapeClass : public OtGuiClass {
public:
	// construct a (sub)path
	OtObject moveTo(float x, float y);
	OtObject lineTo(float x, float y);
	OtObject quadraticCurveTo(float cx, float cy, float x, float y);
	OtObject bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
	OtObject close();

	// add simple geometries
	OtObject circle(float x, float y, float radius, bool clockwise=false);
	OtObject text(OtObject font, const std::string& text);

	// get number of polygons in shape
	size_t getPolygonCount() {
		return paths.size();
	}

	// get Nth polygon
	void getPolygon(std::vector<glm::vec2>& polygon, size_t n, size_t division) {
		return paths[n]->getSpacedPoints(polygon, division);
	}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtShape create();

private:
	std::vector<OtCurvePath> paths;
	OtCurvePath currentPath;
};
