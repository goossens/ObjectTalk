//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtFunction.h"
#include "OtLog.h"
#include "OtObject.h"
#include "OtValue.h"

#include "OtCanvas.h"
#include "OtMathModule.h"


//
//	OtCanvas
//

class OtCanvasClass;
using OtCanvasObject = OtObjectPointer<OtCanvasClass>;

class OtCanvasClass : public OtObjectClass {
public:
	// constructors
	OtCanvasClass() = default;
	OtCanvasClass(const OtCanvas& c) : canvas(c) {}

	// manipulate composite operation
	inline void compositeOperation(int operation) { canvas.compositeOperation(operation); }
	inline void compositeBlendFunc(int sfactor, int dfactor) { canvas.compositeBlendFunc(sfactor, dfactor); }
	inline void compositeBlendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha) { canvas.compositeBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha); }

	// manipulate rendering state
	inline void save() { canvas.save(); }
	inline void restore() { canvas.restore(); }
	inline void reset() { canvas.reset(); }

	// manage images
	inline int loadImage(const std::string& path, int flags) { return canvas.loadImage(path, flags); }
	inline void deleteImage(int image) { canvas.deleteImage(image); }
	inline float getImageWidth(int image) { return canvas.getImageWidth(image); }
	inline float getImageHeight(int image) { return canvas.getImageHeight(image); }

	// manage paints
	inline int createLinearGradient(float sx, float sy, float ex, float ey, const std::string& startColor, const std::string& endColor) { return canvas.createLinearGradient(sx, sy, ex, ey, startColor, endColor); }
	inline int createBoxGradient(float x, float y, float w, float h, float r, float f, const std::string& startColor, const std::string& endColor) { return canvas.createBoxGradient(x, y, w, h, r, f, startColor, endColor); }
	inline int createRadialGradient(float cx, float cy, float inner, float outer, const std::string& startColor, const std::string& endColor) { return canvas.createRadialGradient(cx, cy, inner, outer, startColor, endColor); }
	inline int createTexturePattern(float sx, float sy, float ex, float ey, float angle, int texture, float alpha) { return canvas.createTexturePattern(sx, sy, ex, ey, angle, texture, alpha); }
	inline void deletePaint(int id) { canvas.deletePaint(id); }

	// manage fonts
	inline int loadFont(const std::string& path) { return canvas.loadFont(path); }

	// manipulate styles
	inline void antiAlias(bool on) { canvas.antiAlias(on); }
	inline void strokeColor(const std::string& color) { canvas.strokeColor(color); }
	inline void strokePaint(int id) { canvas.strokePaint(id); }
	inline void fillColor(const std::string& color) { canvas.fillColor(color); }
	inline void fillPaint(int id) { canvas.fillPaint(id); }
	inline void miterLimit(float limit) { canvas.miterLimit(limit); }
	inline void strokeWidth(float w) { canvas.strokeWidth(w); }
	inline void lineCap(int cap) { canvas.lineCap(cap); }
	inline void lineJoin(int join) { canvas.lineJoin(join); }
	inline void globalAlpha(float alpha) { canvas.globalAlpha(alpha); }

	inline void translate(float x, float y) { canvas.translate(x, y); }
	inline void rotate(float angle) { canvas.rotate(angle); }
	inline void scale(float x, float y) { canvas.scale(x, y); }
	inline void skewX(float angle) { canvas.skewX(angle); }
	inline void skewY(float angle) { canvas.skewY(angle); }
	inline void resetTransform() { canvas.resetTransform(); }

	inline void beginPath() { canvas.beginPath(); }
	inline void moveTo(float x, float y) { canvas.moveTo(x, y); }
	inline void lineTo(float x, float y) { canvas.lineTo(x, y); }
	inline void bezierCurveTo(float c1x, float c1y, float c2x, float c2y, float x, float y) { canvas.bezierCurveTo(c1x, c1y, c2x, c2y, x, y); }
	inline void quadraticCurveTo(float cx, float cy, float x, float y) { canvas.quadraticCurveTo(cx, cy, x, y); }
	inline void arcTo(float x1, float y1, float x2, float y2, float radius) { canvas.arcTo(x1, y1, x2, y2, radius); }
	inline void closePath() { canvas.closePath(); }
	inline void pathWinding(int winding) { canvas.pathWinding(NVGwinding(winding)); }

	inline void arc(float cx, float cy, float r, float a0, float a1, int dir) { canvas.arc(cx, cy, r, a0, a1, dir); }
	inline void rect(float x, float y, float w, float h) { canvas.rect(x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float r) { canvas.roundedRect(x, y, w, h, r); }
	inline void ellipse(float cx, float cy, float rx, float ry) { canvas.ellipse(cx, cy, rx, ry); }
	inline void circle(float cx, float cy, float r) { canvas.circle(cx, cy, r); }

	inline void stroke() { canvas.stroke(); }
	inline void fill() { canvas.fill(); }

	inline void drawImage(size_t count, OtObject* parameters) {
		switch (count) {
			case 9:
				canvas.drawImage(
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
				canvas.drawImage(
					parameters[0]->operator int(),
					parameters[1]->operator float(),
					parameters[2]->operator float(),
					parameters[3]->operator float(),
					parameters[4]->operator float());

				break;

			case 3:
				canvas.drawImage(
					parameters[0]->operator int(),
					parameters[1]->operator float(),
					parameters[2]->operator float());

				break;

			default:
				OtLogError("[drawImage expects 3, 5, or 9 arguments (not {})", count);
		}
	}

	inline void fontFace(int id) { canvas.fontFace(id); }
	inline void fontSize(float size) { canvas.fontSize(size); }
	inline void fontBlur(float blur) { canvas.fontBlur(blur); }
	inline void fontLetterSpacing(float spacing) { canvas.fontLetterSpacing(spacing); }
	inline void fontLineHeight(float h) { canvas.fontLineHeight(h); }
	inline void fontAlign(int align) { canvas.fontAlign(align); }
	inline float text(float x, float y, const std::string& s) { return canvas.text(x, y, s); }
	inline void textBox(float x, float y, float w, const std::string& s) { canvas.textBox(x, y, w, s); }

	inline glm::vec2 textSize(const std::string& string) { return canvas.textSize(string); }
	inline glm::vec2 textBoxSize(const std::string& string, float w) { return canvas.textBoxSize(string, w); }

	inline float getWidth() { return canvas.getWidth(); }
	inline float getHeight() { return canvas.getHeight(); }

	// access state
	inline void enable() { canvas.enable(); }
	inline void disable() { canvas.disable(); }
	inline bool isEnabled() { return canvas.isEnabled(); }

	inline void requestRerender() { canvas.requestRerender(); }
	inline void markClean() { canvas.markClean(); }
	inline bool needsRerender() { return canvas.needsRerender(); }

	// access raw canvas
	inline OtCanvas& getCanvas() { return canvas; }

	// get type definition
	static inline OtType getMeta() {
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

			type->set("drawImage", OtFunction::create(&OtCanvasClass::drawImage));

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

private:
	OtCanvas canvas;
};


//
//	Allow OtCanvas as an ObjectTalk value
//

template <>
struct OtValue<OtCanvas> {
	static inline OtObject encode(OtCanvas canvas) {
		return OtCanvasClass(canvas);
	}

	static inline OtCanvas decode(OtObject object) {
		if (object.isKindOf<OtCanvasClass>()) {
			return OtCanvasObject(object)->getCanvas();

		} else {
			OtLogError("Expected a [Canvas], not a [{}]", object.getTypeName());
			return OtCanvas();
		}
	}
};


//
//	Helper functions
//

inline OtObject OtCanvasToObject(OtCanvas canvas) {
	return OtValue<OtCanvas>::encode(canvas);
}

inline OtCanvas OtCanvasFromObject(OtObject object) {
	return OtValue<OtCanvas>::decode(object);
}

inline bool OtCanvasValidateObject(OtObject object) {
	return object.isKindOf<OtCanvasClass>();
}
