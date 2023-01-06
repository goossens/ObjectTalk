//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAppObject.h"
#include "OtCamera.h"
#include "OtFrameBuffer.h"
#include "OtRenderer.h"
#include "OtScene.h"


//
//	OtViewClass
//

class OtViewClass;
typedef std::shared_ptr<OtViewClass> OtView;

class OtViewClass : public OtAppObjectClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// update properties
	OtObject setScreenArea(int x, int y, int w, int h);
	OtObject setScene(OtObject scene);
	OtObject setCamera(OtObject camera);

	// access attributes
	OtObject getScene() { return scene; }
	OtObject getCamera() { return camera; }

	// render content
	void render() override;

	// mouse and keyboard events
	bool onMouseButton(int button, int action, int mods, float xpos, float ypos) override;
	bool onMouseMove(float xpos, float ypos) override;
	bool onMouseDrag(int button, int mods, float xpos, float ypos) override;
	bool onScrollWheel(float dx, float dy) override;
	bool onKey(int key, int mods) override;
	bool onChar(unsigned int codepoint) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtView create();

private:
	// the scene we are looking at
	OtScene scene;

	// camera to render scene
	OtCamera camera;

	// scene renderer
	OtRenderer renderer;

	// view geometry (in percentage of screen/window size)
	float x = 0.0;
	float y = 0.0;
	float w = 100.0;
	float h = 100.0;

	// old mouse position for drag operations
	float xold = 0.0;
	float yold = 0.0;

	// debugging support
	void renderDebugGUI();
	OtCamera debugCamera;
	OtFrameBuffer framebuffer;
};
