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

	// update attributes
	OtObject setID(int id);
	OtObject setScreenArea(int x, int y, int w, int h);
	OtObject setCamera(OtObject camera);
	OtObject setScene(OtObject scene);

	// access attributes
	OtObject getCamera() { return camera; }
	OtObject getScene()  { return scene; }

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
