//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nanovg_bgfx.h"

#include "OtException.h"
#include "OtFunction.h"
#include "OtLog.h"
#include "OtPath.h"

#include "OtCanvas.h"
#include "OtImage.h"
#include "OtPass.h"
#include "OtVec2.h"


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

int OtCanvasClass::createLinearGradient(float sx, float sy, float ex, float ey, const std::string& startColor, const std::string& endColor) {
	auto sc = OtColorParser::toVec4(startColor);
	auto ec = OtColorParser::toVec4(endColor);
	auto paint = nvgLinearGradient(context, sx, sy, ex, ey, nvgRGBAf(sc.r, sc.g, sc.b, sc.a), nvgRGBAf(ec.r, ec.g, ec.b, ec.a));
	return addPaint(paint);
}


//
//	OtCanvasClass::createBoxGradient
//

int OtCanvasClass::createBoxGradient(float x, float y, float w, float h, float r, float f, const std::string& startColor, const std::string& endColor) {
	auto sc = OtColorParser::toVec4(startColor);
	auto ec = OtColorParser::toVec4(endColor);
	auto paint = nvgBoxGradient(context, x, y, w, h, r, f, nvgRGBAf(sc.r, sc.g, sc.b, sc.a), nvgRGBAf(ec.r, ec.g, ec.b, ec.a));
	return addPaint(paint);
}


//
//	OtCanvasClass::createRadialGradient
//

int OtCanvasClass::createRadialGradient(float cx, float cy, float inner, float outer, const std::string& startColor, const std::string& endColor) {
	auto sc = OtColorParser::toVec4(startColor);
	auto ec = OtColorParser::toVec4(endColor);
	auto paint = nvgRadialGradient(context, cx, cy, inner, outer, nvgRGBAf(sc.r, sc.g, sc.b, sc.a), nvgRGBAf(ec.r, ec.g, ec.b, ec.a));
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
//	OtCanvasClass::loadFont
//

int OtCanvasClass::loadFont(const std::string& path) {
	auto id =  nvgCreateFont(context, OtPath::getStem(path).c_str(), path.c_str());

	if (id < 0) {
		OtLogError("Can't load font at [{}]", path);
	}

	return id;
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
	drawImage(image, 0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h), x, y, static_cast<float>(w), static_cast<float>(h));
}

void OtCanvasClass::drawImage(int image, float x, float y, float w, float h) {
	int sw, sh;
	nvgImageSize(context, image, &sw, &sh);
	drawImage(image, 0.0f, 0.0f, static_cast<float>(sw), static_cast<float>(sh), x, y, w, h);
}

void OtCanvasClass::drawImage(int image, float sx, float sy, float sw, float sh, float x, float y, float w, float h) {
	int iw, ih;
	nvgImageSize(context, image, &iw, &ih);

	float ax = w / sw;
	float ay = h / sh;
	NVGpaint imgPaint = nvgImagePattern(context, x - sx * ax, y - sy * ay, static_cast<float>(iw) * ax, static_cast<float>(ih) * ay, 0.0f, image, 1.0f);

	nvgBeginPath(context);
	nvgRect(context, x, y, w, h);
	nvgFillPaint(context, imgPaint);
	nvgFill(context);
}


//
//	OtCanvasClass::textSize
//

OtObject OtCanvasClass::textSize(const std::string& string) {
	float bounds[4];
	nvgTextBounds(context, 0.0f, 0.0f, string.c_str(), nullptr, bounds);
	return OtVec2::create(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}


//
//	OtCanvasClass::textBoxSize
//

OtObject OtCanvasClass::textBoxSize(const std::string& string, float w) {
	float bounds[4];
	nvgTextBoxBounds(context, 0.0f, 0.0f, w, string.c_str(), nullptr, bounds);
	return OtVec2::create(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}


//
//	OtCanvasClass::render
//

void OtCanvasClass::render(OtFrameBuffer& framebuffer, float scale, std::function<void()> renderer) {
	// get dimensions
	width = static_cast<float>(framebuffer.getWidth());
	height = static_cast<float>(framebuffer.getHeight());

	// setup rendering pass
	OtPass pass;
	pass.setClear(framebuffer.hasColorTexture(), framebuffer.hasStencilTexture(), glm::vec4(0.0f));
	pass.setViewMode(OtPass::sequential);
	pass.setFrameBuffer(framebuffer);
	pass.touch();

	// render the canvas
	nvgSetViewId(context, pass.getViewId());
	nvgBeginFrame(context, width, height, 1.0);
	nvgScale(context, scale, scale);

	try {
		renderer();

	} catch (OtException& e) {
		nvgEndFrame(context);
		throw(e);
	}

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

		type->set("save", OtFunction::create(&OtCanvasClass::save));
		type->set("restore", OtFunction::create(&OtCanvasClass::restore));
		type->set("reset", OtFunction::create(&OtCanvasClass::reset));

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
		type->set("bezierCurveTo", OtFunction::create(&OtCanvasClass::bezierCurveTo));
		type->set("quadraticCurveTo", OtFunction::create(&OtCanvasClass::quadraticCurveTo));
		type->set("arcTo", OtFunction::create(&OtCanvasClass::arcTo));
		type->set("closePath", OtFunction::create(&OtCanvasClass::closePath));
		type->set("pathWinding", OtFunction::create(&OtCanvasClass::pathWinding));

		type->set("arc", OtFunction::create(&OtCanvasClass::arc));
		type->set("rect", OtFunction::create(&OtCanvasClass::rect));
		type->set("roundedRect", OtFunction::create(&OtCanvasClass::roundedRect));
		type->set("ellipse", OtFunction::create(&OtCanvasClass::ellipse));
		type->set("circle", OtFunction::create(&OtCanvasClass::circle));

		type->set("stroke", OtFunction::create(&OtCanvasClass::stroke));
		type->set("fill", OtFunction::create(&OtCanvasClass::fill));

		type->set("drawImage", OtFunction::create(&OtCanvasClass::drawImageStub));

		type->set("fontFace", OtFunction::create(&OtCanvasClass::fontFace));
		type->set("fontSize", OtFunction::create(&OtCanvasClass::fontSize));
		type->set("fontBlur", OtFunction::create(&OtCanvasClass::fontBlur));
		type->set("fontLetterSpacing", OtFunction::create(&OtCanvasClass::fontLetterSpacing));
		type->set("fontLineHeight", OtFunction::create(&OtCanvasClass::fontLineHeight));
		type->set("fontAlign", OtFunction::create(&OtCanvasClass::fontAlign));
		type->set("text", OtFunction::create(&OtCanvasClass::text));
		type->set("textBox", OtFunction::create(&OtCanvasClass::textBox));

		type->set("textSize", OtFunction::create(&OtCanvasClass::textSize));
		type->set("textBoxSize", OtFunction::create(&OtCanvasClass::textBoxSize));

		type->set("getWidth", OtFunction::create(&OtCanvasClass::getWidth));
		type->set("getHeight", OtFunction::create(&OtCanvasClass::getHeight));

		type->set("enable", OtFunction::create(&OtCanvasClass::enable));
		type->set("disable", OtFunction::create(&OtCanvasClass::disable));
		type->set("isEnabled", OtFunction::create(&OtCanvasClass::isEnabled));

		type->set("requestRerender", OtFunction::create(&OtCanvasClass::requestRerender));
		type->set("markClean", OtFunction::create(&OtCanvasClass::markClean));
		type->set("needsRerender", OtFunction::create(&OtCanvasClass::needsRerender));
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
			OtLogError("[drawImage expects 3, 5, or 9 arguments (not {})", count);
	}
}
