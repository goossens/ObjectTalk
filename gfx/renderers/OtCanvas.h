//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>
#include <unordered_map>

#include "nanovg.h"

#include "OtObject.h"

#include "OtColorParser.h"
#include "OtFrameBuffer.h"


//
//	OtCanvas
//

class OtCanvasClass;
using OtCanvas = OtObjectPointer<OtCanvasClass>;

class OtCanvasClass : public OtObjectClass {
public:
	// constructor/destructor
	OtCanvasClass();
	~OtCanvasClass();

	// manipulate composite operation
	inline void compositeOperation(int operation) { nvgGlobalCompositeOperation(context, operation); }
	inline void compositeBlendFunc(int operation, int sfactor, int dfactor) { nvgGlobalCompositeBlendFunc(context, sfactor, dfactor); }
	inline void compositeBlendFuncSeparate(int operation, int srcRGB, int dstRGB, int srcAlpha, int dstAlpha) { nvgGlobalCompositeBlendFuncSeparate(context, srcRGB, dstRGB, srcAlpha, dstAlpha); }

	// manipulate rendering state
	inline void save() { nvgSave(context); }
	inline void restore() { nvgRestore(context); }
	inline void reset() { nvgReset(context); }

	// manage images
	int loadImage(const std::string& path, int flags);
	inline void deleteImage(int image) { nvgDeleteImage(context, image); }
	inline float getImageWidth(int image) { int w, h; nvgImageSize(context, image, &w, &h); return float(w); }
	inline float getImageHeight(int image) { int w, h; nvgImageSize(context, image, &w, &h); return float(h); }

	// manage paints
	int createLinearGradient(float sx, float sy, float ex, float ey, const std::string& startColor, const std::string& endColor);
	int createBoxGradient(float x, float y, float w, float h, float r, float f, const std::string& startColor, const std::string& endColor);
	int createRadialGradient(float cx, float cy, float inner, float outer, const std::string& startColor, const std::string& endColor);
	int createTexturePattern(float sx, float sy, float ex, float ey, float angle, int texture, float alpha);
	void deletePaint(int id);

	// manage fonts
	int loadFont(const std::string& path);

	// manipulate styles
	inline void antiAlias(bool enabled) { nvgShapeAntiAlias(context, enabled); }
	inline void strokeColor(const std::string& color) { auto c = OtColorParser::toVec4(color); nvgStrokeColor(context, nvgRGBAf(c.r, c.g, c.b, c.a)); }
	inline void strokePaint(int id) { nvgStrokePaint(context, paints[id]); }
	inline void fillColor(const std::string& color) { auto c = OtColorParser::toVec4(color); nvgFillColor(context, nvgRGBAf(c.r, c.g, c.b, c.a)); }
	inline void fillPaint(int id) { nvgFillPaint(context, paints[id]); }
	inline void miterLimit(float limit) { nvgMiterLimit(context, limit); }
	inline void strokeWidth(float width) { nvgStrokeWidth(context, width); }
	inline void lineCap(int cap) { nvgLineCap(context, cap); }
	inline void lineJoin(int join) { nvgLineJoin(context, join); }
	inline void globalAlpha(float alpha) { nvgGlobalAlpha(context, alpha); }

	inline void translate(float x, float y) { nvgTranslate(context, x, y); }
	inline void rotate(float angle) { nvgRotate(context, angle); }
	inline void scale(float x, float y) { nvgScale(context, x, y); }
	inline void skewX(float angle) { nvgSkewX(context, angle); }
	inline void skewY(float angle) { nvgSkewY(context, angle); }
	inline void resetTransform() { nvgResetTransform(context); }

	inline void beginPath() { nvgBeginPath(context); }
	inline void moveTo(float x, float y) { nvgMoveTo(context, x, y); }
	inline void lineTo(float x, float y) { nvgLineTo(context, x, y); }
	inline void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) { nvgBezierTo(context, c1x, c1y, c2x, c2y, x, y); }
	inline void quadTo(float cx, float cy, float x, float y) { nvgQuadTo(context, cx, cy, x, y); }
	inline void arcTo(float x1, float y1, float x2, float y2, float radius) { nvgArcTo(context, x1, y1, x2, y2, radius); }
	inline void closePath() { nvgClosePath(context); }
	inline void pathWinding(int winding) { nvgPathWinding(context, NVGwinding(winding)); }

	inline void arc(float cx, float cy, float r, float a0, float a1, int dir) { nvgArc(context, cx, cy, r, a0, a1, dir); }
	inline void rect(float x, float y, float w, float h) { nvgRect(context, x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float r) { nvgRoundedRect(context, x, y, w, h, r); }
	inline void ellipse(float cx, float cy, float rx, float ry) { nvgEllipse(context, cx, cy, rx, ry); }
	inline void circle(float cx, float cy, float r) { nvgCircle(context, cx, cy, r); }

	inline void stroke() { nvgStroke(context); }
	inline void fill() { nvgFill(context); }

	void drawImage(int image, float x, float y);
	void drawImage(int image, float x, float y, float w, float h);
	void drawImage(int image, float sx, float sy, float sw, float sh, float x, float y, float w, float h);

	inline void fontFace(int id) { nvgFontFaceId(context, id); }
	inline void fontSize(float size) { nvgFontSize(context, size); }
	inline void fontBlur(float blur) { nvgFontBlur(context, blur); }
	inline void fontLetterSpacing(float spacing) { nvgTextLetterSpacing(context, spacing); }
	inline void fontLineHeight(float height) { nvgTextLineHeight(context, height); }
	inline void fontAlign(int align) { nvgTextAlign(context, align); }
	inline float text(float x, float y, const std::string& s) { return nvgText(context, x, y, s.c_str(), nullptr); }
	inline void textBox(float x, float y, float w, const std::string& s) { nvgTextBox(context, x, y, w, s.c_str(), nullptr); }

	OtObject textSize(const std::string& string);
	OtObject textBoxSize(const std::string& string, float w);

	inline float getWidth() { return width; }
	inline float getHeight() { return height; }

	// access state
	inline void enable() { enabled = true; dirty = true; }
	inline void disable() { enabled = false; }
	inline bool isEnabled() { return enabled; }

	inline void requestRerender() { dirty = true; }
	inline void markClean() { dirty = false; }
	inline bool needsRerender() { return dirty; }

	// render canvas to framebuffer
	void render(OtFrameBuffer& framebuffer, float scale, std::function<void()> renderer);

	// get type definition
	static OtType getMeta();

private:
	// properties
	NVGcontext* context;
	std::unordered_map<int, NVGpaint> paints;

	bool enabled{true};
	bool dirty{true};

	float width;
	float height;

	// helper functions
	int addPaint(const NVGpaint& paint);
	int paintID = 1;

	void drawImageStub(size_t count, OtObject* parameters);
};
