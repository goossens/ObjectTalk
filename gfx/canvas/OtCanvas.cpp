//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtCanvas.h"
#include "OtPass.h"


//
//	OtCanvas::OtCanvas
//

OtCanvas::OtCanvas(int width, int height) {
	update(width, height);
}


//
//	OtCanvas::~OtCanvas
//

OtCanvas::~OtCanvas() {
	clear();
}


//
//	OtCanvas::update
//

void OtCanvas::update(int width, int height) {
	clear();
	context = nvgCreate(1, 0);
}


//
//	OtCanvas::clear
//

void OtCanvas::clear() {
	if (context) {
		nvgDelete(context);
		context = nullptr;
	}
}


//
//	OtCanvas::render
//

void OtCanvas::render(OtFrameBuffer &framebuffer) {
	// setup rendering pass
	OtPass pass;
	pass.touch();
	auto view = pass.getViewId();
	bgfx::setViewMode(view, bgfx::ViewMode::Sequential);
	nvgSetViewId(context, view);
}