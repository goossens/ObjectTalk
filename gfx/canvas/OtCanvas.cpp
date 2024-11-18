//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nanovg_bgfx.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtCanvas.h"
#include "OtPass.h"


//
//	OtCanvasClass::OtCanvasClass
//

OtCanvasClass::OtCanvasClass() {
	context = nvgCreate(1, 0);

	if (!context) {
		OtLogFatal("Internal error: can't create a new canvas context");
	}
}


//
//	OtCanvasClass::~OtCanvasClass
//

OtCanvasClass::~OtCanvasClass() {
	nvgDelete(context);
}


//
//	OtCanvasClass::render
//

void OtCanvasClass::render(OtFrameBuffer& framebuffer, std::function<void(OtCanvas)> renderer) {
	// setup rendering pass
	OtPass pass;
	pass.setClear(framebuffer.hasColorTexture(), framebuffer.hasDepthTexture(), glm::vec4(0.0f), 1.0f);
	pass.setViewMode(OtPass::sequential);
	pass.setFrameBuffer(framebuffer);

	// render the canvas
	nvgSetViewId(context, pass.getViewId());
	nvgBeginFrame(context, framebuffer.getWidth(), framebuffer.getHeight(), 1.0);

	renderer(OtCanvas(this));
	nvgEndFrame(context);
}



//
//	OtCanvasClass::getMeta
//

OtType OtCanvasClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtCanvasClass>("Canvas", OtObjectClass::getMeta());
		type->set("saveState", OtFunction::create(&OtCanvasClass::saveState));
		type->set("restoreState", OtFunction::create(&OtCanvasClass::restoreState));
		type->set("resetState", OtFunction::create(&OtCanvasClass::resetState));

		type->set("antiAlias", OtFunction::create(&OtCanvasClass::antiAlias));
		type->set("strokeColor", OtFunction::create(&OtCanvasClass::strokeColor));
		type->set("fillColor", OtFunction::create(&OtCanvasClass::fillColor));
		type->set("miterLimit", OtFunction::create(&OtCanvasClass::miterLimit));
		type->set("strokeWidth", OtFunction::create(&OtCanvasClass::strokeWidth));
		type->set("lineCap", OtFunction::create(&OtCanvasClass::lineCap));
		type->set("lineJoin", OtFunction::create(&OtCanvasClass::lineJoin));
		type->set("globalAlpha", OtFunction::create(&OtCanvasClass::globalAlpha));

		type->set("translate", OtFunction::create(&OtCanvasClass::translate));
		type->set("rotate", OtFunction::create(&OtCanvasClass::rotate));
		type->set("scale", OtFunction::create(&OtCanvasClass::scale));
		type->set("skewX", OtFunction::create(&OtCanvasClass::skewX));
		type->set("skewY", OtFunction::create(&OtCanvasClass::skewY));
		type->set("resetTransform", OtFunction::create(&OtCanvasClass::resetTransform));

		type->set("beginPath", OtFunction::create(&OtCanvasClass::beginPath));
		type->set("moveTo", OtFunction::create(&OtCanvasClass::moveTo));
		type->set("lineTo", OtFunction::create(&OtCanvasClass::lineTo));
		type->set("bezierTo", OtFunction::create(&OtCanvasClass::bezierTo));
		type->set("quadTo", OtFunction::create(&OtCanvasClass::quadTo));
		type->set("arcTo", OtFunction::create(&OtCanvasClass::arcTo));
		type->set("closePath", OtFunction::create(&OtCanvasClass::closePath));
		type->set("pathWinding", OtFunction::create(&OtCanvasClass::pathWinding));

		type->set("drawArc", OtFunction::create(&OtCanvasClass::drawArc));
		type->set("drawRect", OtFunction::create(&OtCanvasClass::drawRect));
		type->set("drawRoundedRect", OtFunction::create(&OtCanvasClass::drawRoundedRect));
		type->set("drawEllipse", OtFunction::create(&OtCanvasClass::drawEllipse));
		type->set("drawCircle", OtFunction::create(&OtCanvasClass::drawCircle));

		type->set("stroke", OtFunction::create(&OtCanvasClass::stroke));
		type->set("fill", OtFunction::create(&OtCanvasClass::fill));

		type->set("buttCap", OtInteger::create(NVG_BUTT));
		type->set("roundCap", OtInteger::create(NVG_ROUND));
		type->set("squareCap", OtInteger::create(NVG_SQUARE));

		type->set("miterJoin", OtInteger::create(NVG_MITER));
		type->set("roundJoin", OtInteger::create(NVG_ROUND));
		type->set("bevelJoin", OtInteger::create(NVG_BEVEL));

		type->set("ccwWinding", OtInteger::create(NVG_SOLID));
		type->set("cwWinding", OtInteger::create(NVG_HOLE));
	}

	return type;
}
