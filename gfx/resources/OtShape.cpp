//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "fmt/core.h"

#include "OtLog.h"
#include "OtText.h"

#include "OtShape.h"


//
//	OtShape::load
//

void OtShape::load(const std::string& filepath) {
	std::string text;
	OtText::load(filepath, text);

	if (!plutovg_path_parse(path, text.c_str(), -1)) {
		OtLogError("Can't parse SVG path in {}", filepath);
	}
}


//
//	OtShape::save
//

void OtShape::save(const std::string& filepath) {
	std::string text;

	struct Context {
		std::string* output;
		bool empty;
	};

	Context context{&text, true};

	// see if we have any paths
	plutovg_path_traverse(path, [](void* closure, plutovg_path_command_t command, const plutovg_point_t* points, [[maybe_unused]] int npoints) {
		Context* ctx = (Context*) closure;

		switch (command) {
			case PLUTOVG_PATH_COMMAND_MOVE_TO:
				ctx->output->append(fmt::format("M {} {}\n", points[0].x, points[0].y));
				ctx->empty = false;
				break;

			case PLUTOVG_PATH_COMMAND_LINE_TO:
				ctx->output->append(fmt::format("L {} {}\n", points[0].x, points[0].y));
				ctx->empty = false;
				break;

			case PLUTOVG_PATH_COMMAND_CUBIC_TO:
				ctx->output->append(fmt::format("C {} {} {} {} {} {}\n", points[0].x, points[0].y, points[1].x, points[1].y, points[2].x, points[2].y));
				ctx->empty = false;
				break;

			case PLUTOVG_PATH_COMMAND_CLOSE:
				if (!ctx->empty) {
					ctx->output->append("Z\n");
				}

				ctx->empty = true;
				break;
		}
	}, &context);

	// write shape to file
	OtText::save(filepath, text);
}


//
//	OtShape::cloneFrom
//

void OtShape::cloneFrom(const OtShape& shape) {
	plutovg_path_destroy(path);
	path = plutovg_path_clone(shape.path);
	incrementVersion();
}


//
//	OtShape::mergeWith
//

void OtShape::mergeWith(const OtShape& shape) {
	plutovg_path_add_path(path, shape.path, nullptr);
	incrementVersion();
}


//
//	OtShape::text
//

void OtShape::text(OtFont& font, const std::string& text, float size, bool center) {
	auto width = font.getWidth(text, size);
	auto dx = center ? -width / 2.0f : 0.0f;

	font.parseGlyphs(
		text, size,
		[&](float x, float y) { moveTo(dx + x, y); },
		[&](float x, float y) { lineTo(dx + x, y); },
		[&](float cx, float cy, float x, float y) { quadraticCurveTo(dx + cx, cy, dx + x, y); },
		[&](float cx1, float cy1, float cx2, float cy2, float x, float y) { bezierCurveTo(dx + cx1, cy1, dx + cx2, cy2, dx + x, y); });
}


//
//	OtShape::transform
//

void OtShape::transform(float tx, float ty, float r, float sx, float sy) {
	plutovg_matrix_t matrix;
	plutovg_matrix_init_translate(&matrix, tx, ty);
	plutovg_matrix_rotate(&matrix, glm::radians(r));
	plutovg_matrix_scale(&matrix, sx, sy);
	plutovg_path_transform(path, &matrix);
}


//
//	OtShape::translate
//

void OtShape::translate(float tx, float ty) {
	plutovg_matrix_t matrix;
	plutovg_matrix_init_translate(&matrix, tx, ty);
	plutovg_path_transform(path, &matrix);
}


//
//	OtShape::rotate
//

void OtShape::rotate(float r) {
	plutovg_matrix_t matrix;
	plutovg_matrix_init_rotate(&matrix, glm::radians(r));
	plutovg_path_transform(path, &matrix);
}


//
//	OtShape::scale
//

void OtShape::scale(float sx, float sy) {
	plutovg_matrix_t matrix;
	plutovg_matrix_init_scale(&matrix, sx, sy);
	plutovg_path_transform(path, &matrix);
}


//
//	OtShape::shear
//

void OtShape::shear(float sx, float sy) {
	plutovg_matrix_t matrix;
	plutovg_matrix_init_shear(&matrix, sx, sy);
	plutovg_path_transform(path, &matrix);
}


//
//	OtShape::getDimensions
//

void OtShape::getDimensions(float& x, float& y, float& w, float& h, float& length) {
	plutovg_rect_t rect;
	length = plutovg_path_extents(path, &rect, true);
	x = rect.x;
	y = rect.y;
	w = rect.w;
	h = rect.h;
}


//
//	pointToLineDistance
//

static float pointToLineDistance(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	if (dx == 0 && dy == 0) {
		dx = p.x - a.x;
		dy = p.y - a.y;
		return std::sqrt(dx * dx + dy * dy);

	} else {
		float t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / (dx * dx + dy * dy);
		t = std::max(0.0f, std::min(1.0f, t));
		float closestX = a.x + t * dx;
		float closestY = a.y + t * dy;
		dx = p.x - closestX;
		dy = p.y - closestY;
		return std::sqrt(dx * dx + dy * dy);
	}
}


//
//	flattenBezier
//

static void flattenBezier(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, double tolerance, std::vector<glm::vec2>* output) {
	// calculate the maximum distance from control points to the chord
	double dist1 = pointToLineDistance(p1, p0, p3);
	double dist2 = pointToLineDistance(p2, p0, p3);

	// if the curve is flat enough, add the end point to the output
	if (std::max(dist1, dist2) <= tolerance) {
		output->emplace_back(p3);

	} else {
		// otherwise, subdivide the curve and flatten the two halves using de Casteljau's algorithm
		auto q1 = (p0 + p1) / 2.0f;
		auto q2 = (p1 + p2) / 2.0f;
		auto q3 = (p2 + p3) / 2.0f;
		auto r1 = (q1 + q2) / 2.0f;
		auto r2 = (q2 + q3) / 2.0f;
		auto r3 = (r1 + r2) / 2.0f;
		flattenBezier(p0, q1, r1, r3, tolerance, output);
		flattenBezier(r3, r2, q3, p3, tolerance, output);
	}
}


//
//	OtShape::getSegments
//

void OtShape::getSegments(std::vector<glm::vec2>& points, std::vector<size_t>& sizes, float tolerance) {
	// clear output
	points.clear();
	sizes.clear();

	// create configuration for traverser
	struct Config {
		std::vector<glm::vec2>* points;
		std::vector<size_t>* sizes;
		size_t start;
		float tolerance;
	} config{&points, &sizes, 0, std::max(tolerance, 0.001f)};

	plutovg_path_traverse(path, [](void* closure, plutovg_path_command_t command, const plutovg_point_t* points, [[maybe_unused]] int npoints) {
		Config* config = (Config*) closure;
		auto size = config->points->size() - config->start;

		// collect all points on path and flatten segments to tolerance
		if (command == PLUTOVG_PATH_COMMAND_MOVE_TO || command == PLUTOVG_PATH_COMMAND_CLOSE) {
			// segments should be at least three points
			if (size == 1) {
				config->points->pop_back();

			} else if (size == 2) {
				config->points->pop_back();
				config->points->pop_back();

			} else if (size) {
				config->sizes->emplace_back(size);
				config->start = config->points->size();
			}
		}

		if (command == PLUTOVG_PATH_COMMAND_MOVE_TO || command == PLUTOVG_PATH_COMMAND_LINE_TO) {
			config->points->emplace_back(points[0].x, points[0].y);

		} else if (command == PLUTOVG_PATH_COMMAND_CUBIC_TO) {
			if (size) {
				glm::vec2 p0 = config->points->back();
				glm::vec2 p1{points[0].x, points[0].y};
				glm::vec2 p2{points[1].x, points[1].y};
				glm::vec2 p3{points[2].x, points[2].y};
				flattenBezier(p0, p1, p2, p3, config->tolerance, config->points);
			}
		}

	}, &config);

	// add last segment (if required)
	auto size = points.size() - config.start;

	if (size == 1) {
		points.pop_back();

	} else if (size == 2) {
		points.pop_back();
		points.pop_back();

	} else if (size) {
		sizes.emplace_back(size);
	}
}


//
//	surfaceToImage
//

static inline void surfaceToImage(OtImage& image, plutovg_surface_t* surface, int w, int h) {
	// convert ABGR to RGBA and mirror image on Y-axis
	auto src = plutovg_surface_get_data(surface);
	auto dest = (unsigned char*) image.getPixels();
	auto stride = w * 4;

	for (auto y = 0; y < h; y++) {
		auto row = (uint32_t*)(src + stride * (h - y - 1));

		for (auto x = 0; x < w; x++) {
			auto pixel = row[x];
			uint32_t a = (pixel >> 24) & 0xFF;

			if (a == 0) {
				*dest++ = 0;
				*dest++ = 0;
				*dest++ = 0;
				*dest++ = 0;

			} else {
				uint32_t r = (pixel >> 16) & 0xFF;
				uint32_t g = (pixel >> 8) & 0xFF;
				uint32_t b = (pixel >> 0) & 0xFF;

				if (a != 255) {
					r = (r * 255) / a;
					g = (g * 255) / a;
					b = (b * 255) / a;
				}

				*dest++ = static_cast<unsigned char>(r);
				*dest++ = static_cast<unsigned char>(g);
				*dest++ = static_cast<unsigned char>(b);
				*dest++ = static_cast<unsigned char>(a);
			}
		}
	}
}


//
//	OtShape::renderStroke
//

void OtShape::renderStroke(OtImage& image, const OtColor& color, float lineWidth) {
	// size the image to the extent of the shape plus a margin
	static constexpr float margin = 10.0f;
	plutovg_rect_t rect;
	plutovg_path_extents(path, &rect, true);
	auto w = int(rect.w + margin * 2.0f);
	auto h = int(rect.h + margin * 2.0f);
	image.update(w, h, OtImage::Format::rgba8);

	// create the rendering surface and canvas
	plutovg_surface_t* surface = plutovg_surface_create(w, h);
	plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

	// render path to surface
	plutovg_canvas_set_rgba(canvas, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	plutovg_canvas_set_line_width(canvas, lineWidth);
	plutovg_canvas_translate(canvas, -rect.x + margin, -rect.y + margin);
	plutovg_canvas_add_path(canvas, path);
	plutovg_canvas_stroke(canvas);

	// convert rendering surface to image
	surfaceToImage(image, surface, w, h);

	// cleanup
	plutovg_canvas_destroy(canvas);
	plutovg_surface_destroy(surface);
}


//
//	OtShape::renderFill
//

void OtShape::renderFill(OtImage& image, const OtColor& color) {
	// size the image to the extent of the shape plus a margin
	static constexpr float margin = 10.0f;
	plutovg_rect_t rect;
	plutovg_path_extents(path, &rect, true);
	auto w = int(rect.w + margin * 2.0f);
	auto h = int(rect.h + margin * 2.0f);
	image.update(w, h, OtImage::Format::rgba8);

	// create the rendering surface and canvas
	plutovg_surface_t* surface = plutovg_surface_create(w, h);
	plutovg_canvas_t* canvas = plutovg_canvas_create(surface);

	// render path to surface
	plutovg_canvas_set_rgba(canvas, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
	plutovg_canvas_set_fill_rule(canvas, PLUTOVG_FILL_RULE_EVEN_ODD);
	plutovg_canvas_translate(canvas, -rect.x + margin, -rect.y + margin);
	plutovg_canvas_add_path(canvas, path);
	plutovg_canvas_fill(canvas);

	// convert rendering surface to image
	surfaceToImage(image, surface, w, h);

	// cleanup
	plutovg_canvas_destroy(canvas);
	plutovg_surface_destroy(surface);
}
