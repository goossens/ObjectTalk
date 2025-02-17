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
#include <utility>
#include <vector>

#include "glm/glm.hpp"
#include "plutovg.h"

#include "OtColor.h"
#include "OtFont.h"
#include "OtImage.h"


//
//	OtShape
//
//	Shapes are like paths in a Scaleable Vector Graphic (SVG)
//	The difference is that the coordinate system is mirrored
//	on the y-axes. SVG has the positive y-axis going down.
//	Shapes have the positive y-axis going up which ensures
//	that extruded shapes are not shown upside down in a 3D
//	renderering.
//

class OtShape {
public:
	// constructors/destructor (rule of 7)
	OtShape() { path = plutovg_path_create(); }
	OtShape(plutovg_path_t* p) { path = plutovg_path_reference(p); }
	OtShape(const OtShape& shape) { path = plutovg_path_reference(shape.path); version = shape.version; }
	OtShape(OtShape&& shape) { path = std::exchange(shape.path, nullptr); version = shape.version; }
	~OtShape() { plutovg_path_destroy(path); }

	OtShape& operator=(const OtShape& shape) {
		plutovg_path_destroy(path);
		path = plutovg_path_reference(shape.path);
		version = shape.version;
		return *this;
	}

    OtShape& operator=(OtShape&& shape) {
		plutovg_path_destroy(path);
		path = std::exchange(shape.path, nullptr);
		version = shape.version;
		return *this;
	}

	// clear the shape
	inline void clear() {
		plutovg_path_destroy(path);
		path = plutovg_path_create();
		incrementVersion();
	}

	// see if shape is valid
	inline bool isValid() { return plutovg_path_get_elements(path, nullptr) >= 3; }

	// shape I/O
	void load(const std::string& filepath);
	void save(const std::string& filepath);

	// clone a shape
	void cloneFrom(const OtShape& shape);

	// merge 2 shapes
	void mergeWith(const OtShape& shape);

	// construct a path
	inline void moveTo(float x, float y) { plutovg_path_move_to(path, x, y); }
	inline void lineTo(float x, float y) { plutovg_path_line_to(path, x, y); }
	inline void quadraticCurveTo(float cx, float cy, float x, float y) { plutovg_path_quad_to(path, cx, cy, x, y); }
	inline void bezierCurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) { plutovg_path_cubic_to(path, cx1, cy1, cx2, cy2, x, y); }
	inline void arcTo(float rx, float ry, float angle, bool largeArcFlag, bool sweepFlag, float x, float y) { plutovg_path_arc_to(path, rx, ry, angle, largeArcFlag, sweepFlag, x, y); }
	inline void close() { plutovg_path_close(path); }

	// add simple geometries
	inline void rect(float x, float y, float w, float h) { plutovg_path_add_rect(path, x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float rx, float ry) { plutovg_path_add_round_rect(path, x, y, w, h, rx, ry); }
	inline void circle(float x, float y, float r) { plutovg_path_add_circle(path, x, y, r); }
	inline void ellipse(float x, float y, float rx, float ry) { plutovg_path_add_ellipse(path, x, y, rx, ry); }
	inline void arc(float x, float y, float r, float a0, float a1, bool ccb) { plutovg_path_add_arc(path, x, y, r, a0, a1, ccb); }
	void text(OtFont& font, const std::string& text, float size, bool center);

	// transform the shape
	void transform(float tx, float ty, float r, float sx, float sy);
	void translate(float tx, float ty);
	void rotate(float r);
	void scale(float sx, float sy);
	void shear(float sx, float sy);

	// get dimensions of shape
	void getDimensions(float& x, float& y, float& w, float& h, float& length);

	// get shape segments
	void getSegments(std::vector<glm::vec2>& points, std::vector<size_t>& sizes, float tolerance);

	// render shape to an image
	void renderStroke(OtImage& image, const OtColor& color, float lineWidth);
	void renderFill(OtImage& image, const OtColor& color);

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if shapes are identical
	inline bool operator==(OtShape& rhs) {
		return path == rhs.path && version == rhs.version;
	}

	inline bool operator!=(OtShape& rhs) {
		return !operator==(rhs);
	}

private:
	plutovg_path_t* path;
	int version = 0;
};
