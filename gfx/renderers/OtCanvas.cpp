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
#include "OtMathModule.h"


//
//	OtCanvas::OtCanvas
//

OtCanvas::OtCanvas() {
	context = nvgCreate(1, 0);

	if (!context) {
		OtLogFatal("Internal error: can't create a new canvas context");
	}
}


//
//	OtCanvas::~OtCanvas
//

OtCanvas::~OtCanvas() {
	nvgDelete(context);
}


//
//	OtCanvas::loadImage
//

int OtCanvas::loadImage(const std::string &path, int flags) {
	OtImage image{path};
	return nvgCreateImageRGBA(context, image.getWidth(), image.getHeight(), flags, (const unsigned char *) image.getPixels());
}


//
//	OtCanvas::createLinearGradient
//

int OtCanvas::createLinearGradient(float sx, float sy, float ex, float ey, const std::string& startColor, const std::string& endColor) {
	auto sc = OtColorParser::toVec4(startColor);
	auto ec = OtColorParser::toVec4(endColor);
	auto paint = nvgLinearGradient(context, sx, sy, ex, ey, nvgRGBAf(sc.r, sc.g, sc.b, sc.a), nvgRGBAf(ec.r, ec.g, ec.b, ec.a));
	return addPaint(paint);
}


//
//	OtCanvas::createBoxGradient
//

int OtCanvas::createBoxGradient(float x, float y, float w, float h, float r, float f, const std::string& startColor, const std::string& endColor) {
	auto sc = OtColorParser::toVec4(startColor);
	auto ec = OtColorParser::toVec4(endColor);
	auto paint = nvgBoxGradient(context, x, y, w, h, r, f, nvgRGBAf(sc.r, sc.g, sc.b, sc.a), nvgRGBAf(ec.r, ec.g, ec.b, ec.a));
	return addPaint(paint);
}


//
//	OtCanvas::createRadialGradient
//

int OtCanvas::createRadialGradient(float cx, float cy, float inner, float outer, const std::string& startColor, const std::string& endColor) {
	auto sc = OtColorParser::toVec4(startColor);
	auto ec = OtColorParser::toVec4(endColor);
	auto paint = nvgRadialGradient(context, cx, cy, inner, outer, nvgRGBAf(sc.r, sc.g, sc.b, sc.a), nvgRGBAf(ec.r, ec.g, ec.b, ec.a));
	return addPaint(paint);
}


//
//	OtCanvas::deletePaint
//

void OtCanvas::deletePaint(int id) {
	auto entry = paints.find(id);

	if (entry != paints.end()) {
		paints.erase(entry);
	}
}


//
//	OtCanvas::loadFont
//

int OtCanvas::loadFont(const std::string& path) {
	auto id =  nvgCreateFont(context, OtPath::getStem(path).c_str(), path.c_str());

	if (id < 0) {
		OtLogError("Can't load font at [{}]", path);
	}

	return id;
}


//
//	createTexturePattern
//

int OtCanvas::createTexturePattern(float sx, float sy, float ex, float ey, float angle, int texture, float alpha) {
	auto paint = nvgImagePattern(context, sx, sy, ex, ey, angle, texture, alpha);
	return addPaint(paint);
}


//
//	OtCanvas::drawImage
//

void OtCanvas::drawImage(int image, float x, float y) {
	int w, h;
	nvgImageSize(context, image, &w, &h);
	drawImage(image, 0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h), x, y, static_cast<float>(w), static_cast<float>(h));
}

void OtCanvas::drawImage(int image, float x, float y, float w, float h) {
	int sw, sh;
	nvgImageSize(context, image, &sw, &sh);
	drawImage(image, 0.0f, 0.0f, static_cast<float>(sw), static_cast<float>(sh), x, y, w, h);
}

void OtCanvas::drawImage(int image, float sx, float sy, float sw, float sh, float x, float y, float w, float h) {
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
//	OtCanvas::textSize
//

glm::vec2 OtCanvas::textSize(const std::string& string) {
	float bounds[4];
	nvgTextBounds(context, 0.0f, 0.0f, string.c_str(), nullptr, bounds);
	return glm::vec2(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}


//
//	OtCanvas::textBoxSize
//

glm::vec2 OtCanvas::textBoxSize(const std::string& string, float w) {
	float bounds[4];
	nvgTextBoxBounds(context, 0.0f, 0.0f, w, string.c_str(), nullptr, bounds);
	return glm::vec2(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}


//
//	OtCanvas::render
//

void OtCanvas::render(OtFrameBuffer& framebuffer, float scale, std::function<void()> renderer) {
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
//	OtCanvas::addPaint
//

int OtCanvas::addPaint(const NVGpaint& paint) {
	auto id = paintID++;
	paints[id] = paint;
	return id;
}
