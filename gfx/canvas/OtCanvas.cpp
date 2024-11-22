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
#include "OtImage.h"
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
//	OtCanvasClass::loadImage
//

int OtCanvasClass::loadImage(const std::string &path, int flags) {
	OtImage image{path};
	return nvgCreateImageRGBA(context, image.getWidth(), image.getHeight(), flags, (const unsigned char *) image.getPixels());
}


//
//	OtCanvasClass::createLinearGradient
//

int OtCanvasClass::createLinearGradient(float sx, float sy, float ex, float ey, float sr, float sg, float sb, float sa, float er, float eg, float eb, float ea) {
	auto paint = nvgLinearGradient(context, sx, sy, ex, ey, nvgRGBAf(sr, sg, sb, sa), nvgRGBAf(er, eg, eb, ea));
	return addPaint(paint);
}


//
//	OtCanvasClass::createBoxGradient
//

int OtCanvasClass::createBoxGradient(float x, float y, float w, float h, float r, float f, float sr, float sg, float sb, float sa, float er, float eg, float eb, float ea) {
	auto paint = nvgBoxGradient(context, x, y, w, h, r, f, nvgRGBAf(sr, sg, sb, sa), nvgRGBAf(er, eg, eb, ea));
	return addPaint(paint);
}


//
//	OtCanvasClass::createRadialGradient
//

int OtCanvasClass::createRadialGradient(float cx, float cy, float inner, float outer, float sr, float sg, float sb, float sa, float er, float eg, float eb, float ea) {
	auto paint = nvgRadialGradient(context, cx, cy, inner, outer, nvgRGBAf(sr, sg, sb, sa), nvgRGBAf(er, eg, eb, ea));
	return addPaint(paint);
}


//
//	OtCanvasClass::deletePaint
//

void OtCanvasClass::deletePaint(int id) {
	auto entry = paints.find(id);

	if (entry != paints.end()) {
		paints.erase(entry);
	}
}


//
//	createTexturePattern
//

int OtCanvasClass::createTexturePattern(float sx, float sy, float ex, float ey, float angle, int texture, float alpha) {
	auto paint = nvgImagePattern(context, sx, sy, ex, ey, angle, texture, alpha);
	return addPaint(paint);
}


//
//	OtCanvasClass::drawImage
//

void OtCanvasClass::drawImage(int image, float x, float y) {
	int w, h;
	nvgImageSize(context, image, &w, &h);
	drawImage(image, 0.0f, 0.0f, float(w), float(h), x, y, w, h);
}

void OtCanvasClass::drawImage(int image, float x, float y, float w, float h) {
	int sw, sh;
	nvgImageSize(context, image, &sw, &sh);
	drawImage(image, 0.0f, 0.0f, float(sw), float(sh), x, y, w, h);
}

void OtCanvasClass::drawImage(int image, float sx, float sy, float sw, float sh, float x, float y, float w, float h) {
	int iw, ih;
	nvgImageSize(context, image, &iw, &ih);

	float ax = w / sw;
	float ay = h / sh;
	NVGpaint imgPaint = nvgImagePattern(context, x - sx * ax, y - sy * ay, float(iw) * ax, float(ih) * ay, 0.0f, image, 1.0f);

	nvgBeginPath(context);
	nvgRect(context, x, y, w, h);
	nvgFillPaint(context, imgPaint);
	nvgFill(context);
}


//
//	OtCanvasClass::render
//
void OtCanvasClass::render(OtFrameBuffer &framebuffer, std::function<void()> renderer) {
	// setup rendering pass
	OtPass pass;
	pass.setClear(framebuffer.hasColorTexture(), framebuffer.hasDepthTexture(), glm::vec4(0.0f), 1.0f);
	pass.setViewMode(OtPass::sequential);
	pass.setFrameBuffer(framebuffer);

	// render the canvas
	nvgSetViewId(context, pass.getViewId());
	nvgBeginFrame(context, framebuffer.getWidth(), framebuffer.getHeight(), 1.0);

	renderer();
	nvgEndFrame(context);
}

//
//	OtCanvasClass::getMeta
//

OtType OtCanvasClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtCanvasClass>("Canvas", OtObjectClass::getMeta());

		type->set("compositeOperation", OtFunction::create(&OtCanvasClass::compositeOperation));
		type->set("compositeBlendFunc", OtFunction::create(&OtCanvasClass::compositeBlendFunc));
		type->set("compositeBlendFuncSeparate", OtFunction::create(&OtCanvasClass::compositeBlendFuncSeparate));

		type->set("saveState", OtFunction::create(&OtCanvasClass::saveState));
		type->set("restoreState", OtFunction::create(&OtCanvasClass::restoreState));
		type->set("resetState", OtFunction::create(&OtCanvasClass::resetState));

		type->set("loadImage", OtFunction::create(&OtCanvasClass::loadImage));
		type->set("deleteImage", OtFunction::create(&OtCanvasClass::deleteImage));
		type->set("getImageWidth", OtFunction::create(&OtCanvasClass::getImageWidth));
		type->set("getImageHeight", OtFunction::create(&OtCanvasClass::getImageHeight));

		type->set("createLinearGradient", OtFunction::create(&OtCanvasClass::createLinearGradient));
		type->set("createBoxGradient", OtFunction::create(&OtCanvasClass::createBoxGradient));
		type->set("createRadialGradient", OtFunction::create(&OtCanvasClass::createRadialGradient));
		type->set("createTexturePattern", OtFunction::create(&OtCanvasClass::createTexturePattern));
		type->set("deletePaint", OtFunction::create(&OtCanvasClass::deletePaint));

		type->set("loadFont", OtFunction::create(&OtCanvasClass::loadFont));

		type->set("antiAlias", OtFunction::create(&OtCanvasClass::antiAlias));
		type->set("strokeColor", OtFunction::create(&OtCanvasClass::strokeColor));
		type->set("strokePaint", OtFunction::create(&OtCanvasClass::strokePaint));
		type->set("fillColor", OtFunction::create(&OtCanvasClass::fillColor));
		type->set("fillPaint", OtFunction::create(&OtCanvasClass::fillPaint));
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
		type->set("drawImage", OtFunction::create(&OtCanvasClass::drawImageStub));

		type->set("stroke", OtFunction::create(&OtCanvasClass::stroke));
		type->set("fill", OtFunction::create(&OtCanvasClass::fill));

		type->set("fontFace", OtFunction::create(&OtCanvasClass::fontFace));
		type->set("fontSize", OtFunction::create(&OtCanvasClass::fontSize));
		type->set("fontBlur", OtFunction::create(&OtCanvasClass::fontBlur));
		type->set("fontLetterSpacing", OtFunction::create(&OtCanvasClass::fontLetterSpacing));
		type->set("fontLineHeight", OtFunction::create(&OtCanvasClass::fontLineHeight));
		type->set("fontAlign", OtFunction::create(&OtCanvasClass::fontAlign));
		type->set("text", OtFunction::create(&OtCanvasClass::text));
		type->set("textBox", OtFunction::create(&OtCanvasClass::textBox));
	}

	return type;
}


//
//	OtCanvasClass::addPaint
//

int OtCanvasClass::addPaint(const NVGpaint& paint) {
	auto id = paintID++;
	paints[id] = paint;
	return id;
}


//
//	OtCanvasClass::drawImageStub
//

void OtCanvasClass::drawImageStub(size_t count, OtObject* parameters) {
	switch (count) {
		case 9:
			drawImage(
				parameters[0]->operator int(),
				parameters[1]->operator float(),
				parameters[2]->operator float(),
				parameters[3]->operator float(),
				parameters[4]->operator float(),
				parameters[5]->operator float(),
				parameters[6]->operator float(),
				parameters[7]->operator float(),
				parameters[8]->operator float());

			break;

		case 5:
			drawImage(
				parameters[0]->operator int(),
				parameters[1]->operator float(),
				parameters[2]->operator float(),
				parameters[3]->operator float(),
				parameters[4]->operator float());

			break;

		case 3:
			drawImage(
				parameters[0]->operator int(),
				parameters[1]->operator float(),
				parameters[2]->operator float());

			break;

		default:
			OtLogFatal("[drawImage expects 3, 5, or 9 arguments (not {})", count);
	}
}
