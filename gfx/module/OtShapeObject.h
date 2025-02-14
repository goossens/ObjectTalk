//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtObject.h"

#include "OtShape.h"

//
//	OtShapeObject
//

class OtShapeClass;
using OtShapeObject = OtObjectPointer<OtShapeClass>;

class OtShapeClass : public OtObjectClass {
public:
	// clear the shape
	inline void clear() { shape.clear(); }

	// shape I/O
	inline void load(const std::string& path) { shape.load(path); }
	inline void save(const std::string& path) { shape.save(path); }

	// construct a path
	inline void moveTo(float x, float y) { shape.moveTo(x, y); }
	inline void lineTo(float x, float y) { shape.lineTo(x, y); }
	inline void quadraticCurveTo(float cx, float cy, float x, float y) { shape.quadraticCurveTo(cx, cy, x, y); }
	inline void bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) { shape.bezierCurveTo(cx1, cy1, cx2, cy2, x, y); }
	inline void arcTo(float rx, float ry, float angle, bool largeArcFlag, bool sweepFlag, float x, float y) { shape.arcTo(rx, ry, angle, largeArcFlag, sweepFlag, x, y); }
	inline void close() { shape.close(); }

	inline void rect(float x, float y, float w, float h) { shape.rect(x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float rx, float ry) { shape.roundedRect(x, y, w, h, rx, ry); }
	inline void circle(float x, float y, float r) { shape.circle(x, y, r); }
	inline void ellipse(float x, float y, float rx, float ry) { shape.ellipse(x, y, rx, ry); }
	inline void arc(float x, float y, float r, float a0, float a1, bool ccb) { shape.arc(x, y, r, a0, a1, ccb); }

	// manipulate path
	void transform(float tx, float ty, float r, float sx, float sy) { shape.transform(tx, ty, r, sx, sy); }
	void translate(float tx, float ty) { shape.translate(tx, ty); }
	void rotate(float r) { shape.rotate(r); }
	void scale(float sx, float sy) { shape.scale(sx, sy); }
	void shear(float sx, float sy) { shape.shear(sx, sy); }

	// access raw shape
	inline OtShape& getShape() { return shape; }

	// get type definition
	static OtType getMeta();

private:
	OtShape shape;
};
