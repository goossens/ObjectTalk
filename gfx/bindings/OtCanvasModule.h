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
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtCanvasClass>;
	OtCanvasClass() = default;
	OtCanvasClass(const OtCanvas& c) : canvas(c) {}

private:
	// data
	OtCanvas canvas;
};


//
//	Allow OtCanvas as an ObjectTalk value
//

template <>
struct OtValue<OtCanvas> {
	static inline OtObject encode(OtCanvas canvas) {
		return OtCanvasObject::create(canvas);
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
