//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAppObject.h"


//
//	OtPanelClass
//

class OtPanelClass;
typedef std::shared_ptr<OtPanelClass> OtPanel;

class OtPanelClass : public OtAppObjectClass {
public:
	// initialize panel
	void init(int _x, int _y, int _w, int _h) { x = _x; y = _y; w = _w; h = _h; }

	// ensure specified component is allowed as a child
	void validateChild(OtComponent child);

	// render content
	void render();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPanel create();

private:
	int x = 0;
	int y = 0;
	int w = 30;
	int h = 100;
};
