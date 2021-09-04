//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "widget.h"


//
//	OtTronClass
//

class OtTronClass;
typedef std::shared_ptr<OtTronClass> OtTron;

class OtTronClass : public OtWidgetClass {
public:
	// update widget state
    void update();

    // render widget
    void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTron create();

private:
	// create a new set of arcs
	void createArcs();

	// state parameters
	bool opening = false;
	bool closing = true;
	double tronScale = 0.0;
	double tronRotation = 0.0;

	// definition of one arc
	class OtTronArc {
	public:
		double start = 0.0;
		double end = 0.0;
		double radius = 300.0;
		double width = 5.0;
		double rotation = 0.0;
		ImU32 color;
		bool clockwise = true;
	};

	std::vector<OtTronArc> arcs;
};