//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#include "glm/glm.hpp"
#include "nanovg.h"

#include "OtColorParser.h"
#include "OtFrameBuffer.h"


//
//	OtCanvas
//

class OtCanvas {
public:
	// constructor/destructor
	OtCanvas();
	~OtCanvas();

	// image repeat options
	static constexpr int imageRepeatX = NVG_IMAGE_REPEATX;
	static constexpr int imageRepeatY = NVG_IMAGE_REPEATY;
	static constexpr int imageFlipY = NVG_IMAGE_FLIPY;
	static constexpr int imageNearest = NVG_IMAGE_NEAREST;

	// composite operations
	static constexpr int compositeSourceOver = NVG_SOURCE_OVER;
	static constexpr int compositeSourceIn = NVG_SOURCE_IN;
	static constexpr int compositeSourceOut = NVG_SOURCE_OUT;
	static constexpr int compositeSourceAtop = NVG_ATOP;
	static constexpr int compositeDestinationOver = NVG_DESTINATION_OVER;
	static constexpr int compositeDestinationIn = NVG_DESTINATION_IN;
	static constexpr int compositeDestinationOut = NVG_DESTINATION_OUT;
	static constexpr int compositeDestinationAtop = NVG_DESTINATION_ATOP;
	static constexpr int compositeLighter = NVG_LIGHTER;
	static constexpr int compositeCopy = NVG_COPY;
	static constexpr int compositeXor = NVG_XOR;

	// blend factors
	static constexpr int blendZero = NVG_ZERO;
	static constexpr int blendOne = NVG_ONE;
	static constexpr int blendSourceColor = NVG_SRC_COLOR;
	static constexpr int blendOneMinusSourceColor = NVG_ONE_MINUS_SRC_COLOR;
	static constexpr int blendDestinationColor = NVG_DST_COLOR;
	static constexpr int blendDneMinusDestinationColor = NVG_ONE_MINUS_DST_COLOR;
	static constexpr int blendSourceAlpha = NVG_SRC_ALPHA;
	static constexpr int blendOneMinusSourceAlpha = NVG_ONE_MINUS_SRC_ALPHA;
	static constexpr int blendDestinationAlpha = NVG_DST_ALPHA;
	static constexpr int blendOneMinusDestinationAlpha = NVG_ONE_MINUS_DST_ALPHA;
	static constexpr int blendAlphaSaturate = NVG_SRC_ALPHA_SATURATE;

	// line caps
	static constexpr int lineButtCap = NVG_BUTT;
	static constexpr int lineRoundCap = NVG_ROUND;
	static constexpr int lineSquareCap = NVG_SQUARE;

	// winding order
	static constexpr int windingCcw = NVG_CCW;
	static constexpr int windingCw = NVG_CW;

	// alignment options
	static constexpr int alignLeft = NVG_ALIGN_LEFT;
	static constexpr int alignCenter = NVG_ALIGN_CENTER;
	static constexpr int alignRight = NVG_ALIGN_RIGHT;
	static constexpr int alignTop = NVG_ALIGN_TOP;
	static constexpr int alignMiddle = NVG_ALIGN_MIDDLE;
	static constexpr int alignBottom = NVG_ALIGN_BOTTOM;
	static constexpr int alignBaseline = NVG_ALIGN_BASELINE;

	// manipulate composite operation
	inline void compositeOperation(int operation) { nvgGlobalCompositeOperation(context, operation); }
	inline void compositeBlendFunc(int sfactor, int dfactor) { nvgGlobalCompositeBlendFunc(context, sfactor, dfactor); }
	inline void compositeBlendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha) { nvgGlobalCompositeBlendFuncSeparate(context, srcRGB, dstRGB, srcAlpha, dstAlpha); }

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
	inline void antiAlias(bool on) { nvgShapeAntiAlias(context, on); }
	inline void strokeColor(const std::string& color) { auto c = OtColorParser::toVec4(color); nvgStrokeColor(context, nvgRGBAf(c.r, c.g, c.b, c.a)); }
	inline void strokeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { nvgStrokeColor(context, nvgRGBA(r, g, b, a)); }
	inline void strokeColor(float r, float g, float b, float a) { nvgStrokeColor(context, nvgRGBAf(r, g, b, a)); }
	inline void strokePaint(int id) { nvgStrokePaint(context, paints[id]); }
	inline void fillColor(const std::string& color) { auto c = OtColorParser::toVec4(color); nvgFillColor(context, nvgRGBAf(c.r, c.g, c.b, c.a)); }
	inline void fillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { nvgFillColor(context, nvgRGBA(r, g, b, a)); }
	inline void fillColor(float r, float g, float b, float a) { nvgFillColor(context, nvgRGBAf(r, g, b, a)); }
	inline void fillPaint(int id) { nvgFillPaint(context, paints[id]); }
	inline void miterLimit(float limit) { nvgMiterLimit(context, limit); }
	inline void strokeWidth(float w) { nvgStrokeWidth(context, w); }
	inline void lineCap(int cap) { nvgLineCap(context, cap); }
	inline void lineJoin(int join) { nvgLineJoin(context, join); }
	inline void globalAlpha(float alpha) { nvgGlobalAlpha(context, alpha); }

	// manipulate transform
	inline void translate(float x, float y) { nvgTranslate(context, x, y); }
	inline void rotate(float angle) { nvgRotate(context, angle); }
	inline void scale(float x, float y) { nvgScale(context, x, y); }
	inline void skewX(float angle) { nvgSkewX(context, angle); }
	inline void skewY(float angle) { nvgSkewY(context, angle); }
	inline void resetTransform() { nvgResetTransform(context); }

	// manipulate path
	inline void beginPath() { nvgBeginPath(context); }
	inline void moveTo(float x, float y) { nvgMoveTo(context, x, y); }
	inline void lineTo(float x, float y) { nvgLineTo(context, x, y); }
	inline void bezierCurveTo(float c1x, float c1y, float c2x, float c2y, float x, float y) { nvgBezierTo(context, c1x, c1y, c2x, c2y, x, y); }
	inline void quadraticCurveTo(float cx, float cy, float x, float y) { nvgQuadTo(context, cx, cy, x, y); }
	inline void arcTo(float x1, float y1, float x2, float y2, float radius) { nvgArcTo(context, x1, y1, x2, y2, radius); }
	inline void closePath() { nvgClosePath(context); }
	inline void pathWinding(int winding) { nvgPathWinding(context, NVGwinding(winding)); }

	inline void arc(float cx, float cy, float r, float a0, float a1, int dir) { nvgArc(context, cx, cy, r, a0, a1, dir); }
	inline void rect(float x, float y, float w, float h) { nvgRect(context, x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float r) { nvgRoundedRect(context, x, y, w, h, r); }
	inline void ellipse(float cx, float cy, float rx, float ry) { nvgEllipse(context, cx, cy, rx, ry); }
	inline void circle(float cx, float cy, float r) { nvgCircle(context, cx, cy, r); }

	// render path
	inline void stroke() { nvgStroke(context); }
	inline void fill() { nvgFill(context); }

	void drawImage(int image, float x, float y);
	void drawImage(int image, float x, float y, float w, float h);
	void drawImage(int image, float sx, float sy, float sw, float sh, float x, float y, float w, float h);

	inline void fontFace(int id) { nvgFontFaceId(context, id); }
	inline void fontSize(float size) { nvgFontSize(context, size); }
	inline void fontBlur(float blur) { nvgFontBlur(context, blur); }
	inline void fontLetterSpacing(float spacing) { nvgTextLetterSpacing(context, spacing); }
	inline void fontLineHeight(float h) { nvgTextLineHeight(context, h); }
	inline void fontAlign(int align) { nvgTextAlign(context, align); }
	inline float text(float x, float y, const std::string& s) { return nvgText(context, x, y, s.c_str(), nullptr); }
	inline void textBox(float x, float y, float w, const std::string& s) { nvgTextBox(context, x, y, w, s.c_str(), nullptr); }

	glm::vec2 textSize(const std::string& string);
	glm::vec2 textBoxSize(const std::string& string, float w);

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

private:
	// properties
	NVGcontext* context;
	std::unordered_map<int, NVGpaint> paints;

	bool enabled = true;
	bool dirty = true;

	float width;
	float height;

	// helper functions
	int addPaint(const NVGpaint& paint);
	int paintID = 1;
};
