//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "camera.h"
#include "scene.h"
#include "widget.h"


//
//	OtViewClass
//

class OtViewClass;
typedef std::shared_ptr<OtViewClass> OtView;

class OtViewClass : public OtWidgetClass {
public:
	// destructor
	~OtViewClass();

	// initialize view
	void init(int _id, int _x, int _y, int _w, int _h) { id = _id; x = _x; y = _y; w = _w; h = _h; }

	// specifiy camera for view
	void setCamera(OtObject camera);

	// specifiy scene for view
	void setScene(OtObject scene);

	// render content
	void render();

	// mouse and keyboard events
	void onMouseButton(int button, int action, int mods, double xpos, double ypos);
	void onMouseMove(int button, double xpos, double ypos);
	void onKey(int key, int mods);
	void onChar(unsigned int codepoint);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtView create();

protected:
	// view identifier
	int id = 10;

	// our camera
	OtCamera camera;

	// the scene we are looking at
	OtScene scene;

	// view geometry
	float x = 0;
	float y = 0;
	float w = 100;
	float h = 100;

	// old mouse postion for drag operation
	double xold = 0.0;
	double yold = 0.0;
};
