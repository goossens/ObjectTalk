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
#include "plutovg.h"

#include "OtColorParser.h"
#include "OtImage.h"


//
//	OtImageCanvas
//

class OtImageCanvas {
public:
	// constructor/destructor
	OtImageCanvas(int w, int h);
	~OtImageCanvas();

	// manipulate rendering state
	inline void save() { plutovg_canvas_save(canvas); }
	inline void restore() { plutovg_canvas_restore(canvas); }

	// manipulate styles
	inline void strokeColor(const std::string& color) { auto c = OtColorParser::toVec4(color); plutovg_canvas_set_rgba(canvas, c.r, c.g, c.b, c.a); }
	inline void strokeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { plutovg_canvas_set_rgba(canvas, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
	inline void strokeColor(float r, float g, float b, float a) { plutovg_canvas_set_rgba(canvas, r, g, b, a); }
	inline void fillColor(const std::string& color) { auto c = OtColorParser::toVec4(color); plutovg_canvas_set_rgba(canvas, c.r, c.g, c.b, c.a); }
	inline void fillColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { plutovg_canvas_set_rgba(canvas, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); }
	inline void fillColor(float r, float g, float b, float a) { plutovg_canvas_set_rgba(canvas, r, g, b, a); }

	// manipulate transform
	inline void translate(float x, float y) { plutovg_canvas_translate(canvas, x, y); }
	inline void rotate(float angle) { plutovg_canvas_rotate(canvas, angle); }
	inline void scale(float x, float y) { plutovg_canvas_scale(canvas, x, y); }
	inline void shear(float x, float y) { plutovg_canvas_shear(canvas, x, y); }
	inline void resetTransform() { plutovg_canvas_reset_matrix(canvas); }

	// manipulate path
	inline void beginPath() { plutovg_canvas_new_path(canvas); }
	inline void moveTo(float x, float y) { plutovg_canvas_move_to(canvas, x, y); }
	inline void lineTo(float x, float y) { plutovg_canvas_line_to(canvas, x, y); }
	inline void bezierCurveTo(float c1x, float c1y, float c2x, float c2y, float x, float y) { plutovg_canvas_cubic_to(canvas, c1x, c1y, c2x, c2y, x, y); }
	inline void quadraticCurveTo(float cx, float cy, float x, float y) { plutovg_canvas_quad_to(canvas, cx, cy, x, y); }
//	inline void arcTo(float x1, float y1, float x2, float y2, float radius) { plutovg_canvas_arc_to(canvas, x1, y1, x2, y2, radius); }
	inline void closePath() { plutovg_canvas_close_path(canvas); }

	inline void arc(float cx, float cy, float r, float a0, float a1, int dir) { plutovg_canvas_arc(canvas, cx, cy, r, a0, a1, dir); }
	inline void rect(float x, float y, float w, float h) { plutovg_canvas_rect(canvas, x, y, w, h); }
	inline void roundedRect(float x, float y, float w, float h, float r) { plutovg_canvas_round_rect(canvas, x, y, w, h, r, r); }
	inline void ellipse(float cx, float cy, float rx, float ry) { plutovg_canvas_ellipse(canvas, cx, cy, rx, ry); }
	inline void circle(float cx, float cy, float r) { plutovg_canvas_circle(canvas, cx, cy, r); }

	// render path
	inline void stroke() { plutovg_canvas_stroke(canvas); }
	inline void fill() { plutovg_canvas_fill(canvas); }

	// render canvas to image
	void render(OtImage& image);

	private:
	plutovg_surface_t* surface;
	plutovg_canvas_t* canvas;
};
