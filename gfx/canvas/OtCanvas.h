//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nanovg.h"

#include "OtFrameBuffer.h"


//
//	OtCanvas
//

class OtCanvas {
public:
	// constructors/destructor
	OtCanvas(int width, int height);
	~OtCanvas();

	// update canvas size
	void update(int width, int height);

	// release canvas data
	void clear();

	// render canvas to dramebuffer
	void render(OtFrameBuffer& framebuffer);
private:
	// properties
	int width;
	int height;
	NVGcontext* context = nullptr;
};
