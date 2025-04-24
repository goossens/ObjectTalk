//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cstdint>
#include <cmath>

#include "OtLog.h"
#include "OtNumbers.h"

#include "OtPass.h"
#include "OtSimplexFont.h"
#include "OtOscilloscope.h"


//
//	OtOscilloscope::drawLine
//

void OtOscilloscope::drawLine(float x0, float y0, float x1, float y1, float lineWidth, uint32_t color) {
	beginDraw(x0, y0);
	drawTo(x1, y1);
	endDraw(lineWidth, color);
}


//
//	OtOscilloscope::drawRectangle
//

void OtOscilloscope::drawRectangle(float x, float y, float w, float h, float lineWidth, uint32_t color) {
	beginDraw(x, y);
	drawTo(x + w, y);
	drawTo(x + w, y + h);
	drawTo(x, y + h);
	drawTo(x, y);
	endDraw(lineWidth, color);
}


//
//	OtOscilloscope::drawCircle
//

void OtOscilloscope::drawCircle(float x, float y, float radius, float steps, float lineWidth, uint32_t color) {
	beginDraw(x, y - radius);

	float step = static_cast<float>(std::numbers::pi) * 2.0f / steps;

	for (float angle = step; angle < static_cast<float>(2.0 * std::numbers::pi - 0.001); angle += step) {
		drawTo(x + radius * std::sin(angle), y - radius * std::cos(angle));
	}

	drawTo(x, y - radius);
	endDraw(lineWidth, color);
}


//
//	OtOscilloscope::drawSevenSegment
//

void OtOscilloscope::drawSevenSegment(float x, float y, float size, const std::string& text, float lineWidth, uint32_t color) {
	// calculate scaling
	auto s = size / 100.0f;
	auto t = (origin == Origin::topLeft) ? s : -s;

	// render each digit
	for (auto& c : text) {
		// get segment pattern
		int pattern = 0;

		switch (c) {
			case ' ': pattern = 0b0000000; break;
			case '0': pattern = 0b0111111; break;
			case '1': pattern = 0b0000110; break;
			case '2': pattern = 0b1011011; break;
			case '3': pattern = 0b1001111; break;
			case '4': pattern = 0b1100110; break;
			case '5': pattern = 0b1101101; break;
			case '6': pattern = 0b1111101; break;
			case '7': pattern = 0b0000111; break;
			case '8': pattern = 0b1111111; break;
			case '9': pattern = 0b1100111; break;
			case 'A': pattern = 0b1110111; break;
			case 'B': pattern = 0b1111111; break;
			case 'C': pattern = 0b0111001; break;
			case 'D': pattern = 0b0111111; break;
			case 'E': pattern = 0b1111001; break;
			case 'F': pattern = 0b1110001; break;
			case 'G': pattern = 0b1111101; break;
			case 'H': pattern = 0b1110110; break;
			case 'I': pattern = 0b0000110; break;
			case 'J': pattern = 0b0011110; break;
			case 'K': pattern = 0b1110000; break;
			case 'L': pattern = 0b0111000; break;
			case 'M': pattern = 0b0110111; break;
			case 'N': pattern = 0b0110111; break;
			case 'O': pattern = 0b0111111; break;
			case 'P': pattern = 0b1110011; break;
			case 'Q': pattern = 0b0111111; break;
			case 'R': pattern = 0b1110111; break;
			case 'S': pattern = 0b1101101; break;
			case 'T': pattern = 0b0000111; break;
			case 'U': pattern = 0b0111110; break;
			case 'V': pattern = 0b0111110; break;
			case 'W': pattern = 0b0111110; break;
			case 'X': pattern = 0b1110000; break;
			case 'Y': pattern = 0b1110010; break;
			case 'Z': pattern = 0b1011011; break;
			case '-': pattern = 0b1000000; break;
			default: pattern = 0b0000000; break;
		}

		// segment A
		if (pattern & 0b0000001) {
			beginDraw(x + 2.7f * s, y + 0.0f);
			drawTo(x + 49.0f * s, y + 0.0f);
			drawTo(x + 49.0f * s, y + 0.5f * t);
			drawTo(x + 41.0f * s, y + 8.0f * t);
			drawTo(x + 9.8f * s, y + 8.0f * t);
			drawTo(x + 2.2f * s, y + 0.5f * t);
			drawTo(x + 2.7f * s, y + 0.0f);
			endDraw(lineWidth, color);
		}

		// segment B
		if (pattern & 0b0000010) {
			beginDraw(x + 50.0f * s, y + 2.7f * t);
			drawTo(x + 50.0f * s, y + 49.0f * t);
			drawTo(x + 46.0f * s, y + 49.0f * t);
			drawTo(x + 42.0f * s, y + 45.0f * t);
			drawTo(x + 42.0f * s, y + 9.8f * t);
			drawTo(x + 50.0f * s, y + 2.2f * t);
			drawTo(x + 50.0f * s, y + 2.7f * t);
			endDraw(lineWidth, color);
		}

		// segment C
		if (pattern & 0b0000100) {
			beginDraw(x + 50.0f * s, y + 98.0f * t);
			drawTo(x + 42.0f * s, y + 91.0f * t);
			drawTo(x + 42.0f * s, y + 55.0f * t);
			drawTo(x + 46.0f * s, y + 51.0f * t);
			drawTo(x + 50.0f * s, y + 51.0f * t);
			drawTo(x + 50.0f * s, y + 98.0f * t);
			endDraw(lineWidth, color);
		}

		// segment D
		if (pattern & 0b0001000) {
			beginDraw(x + 2.2f * s, y + 100.0f * t);
			drawTo(x + 9.8f * s, y + 92.0f * t);
			drawTo(x + 41.0f * s, y + 92.0f * t);
			drawTo(x + 49.0f * s, y + 100.0f * t);
			drawTo(x + 2.7f * s, y + 100.0f * t);
			drawTo(x + 2.2f * s, y + 100.0f * t);
			endDraw(lineWidth, color);
		}

		// segment E
		if (pattern & 0b0010000) {
			beginDraw(x + 0.0f, y + 98.0f * t);
			drawTo(x + 0.0f, y + 51.0f * t);
			drawTo(x + 4.0f * s, y + 51.0f * t);
			drawTo(x + 8.0f * s, y + 55.0f * t);
			drawTo(x + 8.0f * s, y + 91.0f * t);
			drawTo(x + 0.5f * s, y + 98.0f * t);
			drawTo(x + 0.0f, y + 98.0f * t);
			endDraw(lineWidth, color);
		}

		// segment F
		if (pattern & 0b0100000) {
			beginDraw(x + 0.5f * s, y + 2.2f * t);
			drawTo(x + 8.0f * s, y + 9.8f * t);
			drawTo(x + 8.0f * s, y + 45.0f * t);
			drawTo(x + 4.0f * s, y + 49.0f * t);
			drawTo(x + 0.0f * s, y + 49.0f * t);
			drawTo(x + 0.0f * s, y + 2.7f * t);
			drawTo(x + 0.5f * s, y + 2.2f * t);
			endDraw(lineWidth, color);
		}

		// segment G
		if (pattern & 0b1000000) {
			beginDraw(x + 10.2f * s, y + 46.0f * t);
			drawTo(x + 40.0f * s, y + 46.0f * t);
			drawTo(x + 44.0f * s, y + 50.0f * t);
			drawTo(x + 40.0f * s, y + 54.0f * t);
			drawTo(x + 10.2f * s, y + 54.0f * t);
			drawTo(x + 6.2f * s, y + 50.0f * t);
			drawTo(x + 10.2f * s, y + 46.0f * t);
			endDraw(lineWidth, color);
		}

		x += size * 0.8f;
	}
}


//
//	OtOscilloscope::drawText
//

void OtOscilloscope::drawText(float x, float y, float size, const std::string& text, float lineWidth, uint32_t color) {
	auto scaleX = size / 32.0f;
	auto scaleY = scaleX * ((origin == Origin::topLeft) ? -1.0f : 1.0f);

	for (auto& c : text) {
		if (c >= 32 && c <= 126) {
			const int8_t* chr = simplex[c - 32];
			const int8_t* p = chr + 2;
			bool isDrawing = false;

			for (auto i = 0; i < chr[0]; i++) {
				int vx = *p++;
				int vy = *p++;

				if (vx == -1 && vy == -1) {
					if (isDrawing) {
						endDraw(lineWidth, color);
						isDrawing = false;
					}

				} else if (!isDrawing) {
					beginDraw(x + vx * scaleX, y + vy * scaleY);
					isDrawing = true;

				} else {
					drawTo(x + vx * scaleX, y + vy * scaleY);
				}
			}

			if (isDrawing) {
				endDraw(lineWidth, color);
			}

			x += chr[1] * scaleX;
		}
	}
}


//
//	OtOscilloscope::render
//

void OtOscilloscope::render(OtFrameBuffer& framebuffer) {
	// get dimensions
	width = framebuffer.getWidth();
	height = framebuffer.getHeight();

	// update vertex buffers (if required)
	if (vertexBuffers.size() != decaySteps) {
		vertexBuffers.resize(decaySteps);
		vertexBufferSizes.resize(decaySteps);
	}

	// update framebuffers
	blur1.update(width / 3, height / 3);
	blur2.update(width / 3, height / 3);

	// create brush (if required)
	if (!brush.isValid()) {
		uint8_t pixels[brushSize * brushSize * 4];
		uint8_t* p = pixels;

		for (auto y = -halfBrushSize; y < halfBrushSize; y++) {
			for (auto x = -halfBrushSize; x < halfBrushSize; x++) {
				*p++ = 0xff;
				*p++ = 0xff;
				*p++ = 0xff;
				auto distance = std::min(1.0, std::sqrt(static_cast<double>(x * x + y * y)) / static_cast<double>(halfBrushSize));
				*p++ = static_cast<uint8_t>(std::clamp(std::pow(16.0, -2.0 * distance), 0.0, 1.0) * 255.0);
			}
		}

		brush.loadFromMemory(brushSize, brushSize, OtTexture::rgba8Texture, pixels);
	}

	OtPass pass;
	pass.setClear(true);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(framebuffer);

	glm::mat4 matrix;

	switch (origin) {
		case Origin::topLeft:
			matrix = glm::ortho(0.0f, float(width), float(height), 0.0f);
			break;

		case Origin::bottomLeft:
			matrix = glm::ortho(0.0f, float(width), 0.0f, float(height));
			break;

		case Origin::center:
			matrix = glm::ortho(float(-width / 2), float(width / 2), float(-height / 2), float(height / 2));
			break;
	}

	pass.setTransform(glm::mat4(1.0f), matrix);

	// advance to next draw step
	currentDrawStep = (currentDrawStep + 1) % decaySteps;

	// update the vertex buffer
	if (vertices.size()) {
		vertexBuffers[currentDrawStep].set(vertices.data(), static_cast<uint32_t>(vertices.size()), OtVertexPosUvCol2D::getLayout());
	}

	vertexBufferSizes[currentDrawStep] = static_cast<uint32_t>(vertices.size());

	// render all decay steps
	for (size_t c = 0; c < decaySteps; c++) {
		size_t stepi = decaySteps - c - 1;
		size_t i = (currentDrawStep + decaySteps - stepi) % decaySteps;

		// don't render empty buffers
		if (vertexBufferSizes[i] != 0) {
			float alpha;

			if (stepi == 0) {
				alpha = 1.0f;

			} else if (stepi == 1) {
				alpha = decayStart;

			} else {
				alpha = std::pow(decayValue, stepi - 1.0f) * decayStart;
			}

			uniform.setValue(0, glm::vec4(alpha, 0.0f, 0.0f, 0.0f));
			uniform.submit();
			sampler.submit(0, brush);
			vertexBuffers[i].submit();

			shader.setState(
				OtStateWriteRgb |
				OtStateWriteA |
				OtStateBlendAlpha |
				BGFX_STATE_BLEND_EQUATION_SEPARATE(BGFX_STATE_BLEND_EQUATION_ADD, BGFX_STATE_BLEND_EQUATION_MAX));

			pass.runShaderProgram(shader);
		}
	}

	vertices.clear();

	// apply glow
	blur.setIntensity(1.05f + ((brightness - 1.0f) / 5.0f));
	blur.setAlpha(1.0f);

	for (auto p = 0; p < 4; p++) {
		// run horizontal blur
		blur.setDirection(glm::vec2(1.0f, 0.0f));
		blur.render(p == 0 ? framebuffer : blur2, blur1);

		// run vertical blur
		blur.setDirection(glm::vec2(0.0f, 1.0f));
		blur.render(blur1, blur2);
	}

	// combine original rendering with glow
	blit.setState(
		OtStateWriteRgb |
		OtStateWriteA |
		OtStateBlendAdd);

	blit.setIntensity(1.25f + ((brightness - 1.0f) / 2.0f));
	blit.render(blur2, framebuffer);
}


//
//	OtOscilloscope::beginDraw
//

void OtOscilloscope::beginDraw(float x, float y) {
	points.emplace_back(x, y);
}


//
//	OtOscilloscope::drawTo
//

void OtOscilloscope::drawTo(float x, float y) {
	points.emplace_back(x, y);
}


//
//	normalizeAngle
//

static inline float normalizeAngle(float angle) {
	float wrap = 2.0f * static_cast<float>(std::numbers::pi);
	float result = std::fmod(angle, wrap);
	return result < 0.0f ? result + wrap : result;
}


//
//	OtOscilloscope::endDraw
//

void OtOscilloscope::endDraw(float lineWidth, uint32_t color) {
	if (points.size() < 2) {
		OtLogFatal("Internal error: invalid number of points in VectorDisplay draw action");
	}

	// build a list of lines from the list of points
	struct Line {
		float x0, y0, x1, y1;					  // nominal points
		float a;								  // angle
		float sin_a, cos_a;						  // precomputed trig

		float xl0, yl0, xl1, yl1;				  // left side of the box
		float xr0, yr0, xr1, yr1;				  // right side of the box

		bool is_first, is_last;
		bool has_next, has_prev;				  // booleans indicating whether this line connects to prev/next

		float xlt0, ylt0, xlt1, ylt1;			  // coordinates of endcaps (if !has_prev/!has_next)
		float xrt0, yrt0, xrt1, yrt1;			  // coordinates of endcaps (if !has_prev/!has_next)

		float tl0, tl1, tr0, tr1;

		float s0, s1;							  // shorten line by this amount

		float len;
	};

	size_t nlines = points.size() - 1;
	Line* lines = new Line[nlines];

	float t = std::max((0.01f * (width + height) / 2.0f) * lineWidth / 2.0f, 6.0f);
	bool firstIsLast = std::abs(points[0].x - points[nlines].x) < 0.1f && std::abs(points[0].y - points[nlines].y) < 0.1f;

	// compute basics
	for (size_t i = 1; i < points.size(); i++) {
		Line* line = &lines[i - 1];
		line->is_first = i == 1;
		line->is_last = i == nlines;

		// precomputed info for current line
		line->x0 = points[i - 1].x;
		line->y0 = points[i - 1].y;
		line->x1 = points[i].x;
		line->y1 = points[i].y;
		line->a		= std::atan2(line->y1 - line->y0, line->x1 - line->x0);
		line->sin_a = std::sin(line->a);
		line->cos_a = std::cos(line->a);
		line->len	= std::sqrt((line->x1 - line->x0) * (line->x1 - line->x0) + (line->y1 - line->y0) * (line->y1 - line->y0));

		// figure out what connections we have
		line->has_prev = (!line->is_first || (line->is_first && firstIsLast));
		line->has_next = (!line->is_last || (line->is_last && firstIsLast));

		// initialize to default values
		line->tl0 = line->tl1 = line->tr0 = line->tr1 = t;
		line->s0 = line->s1 = 0.0f;
	}

	// compute adjustments for connected line segments
	for (size_t i = 0; i < nlines; i++) {
		Line* line = &lines[i];
		Line* pline = &lines[(nlines + i - 1) % nlines];

		if (line->has_prev) {
			float pa2a = normalizeAngle(pline->a - line->a);
			float a2pa = normalizeAngle(line->a - pline->a);
			float maxshorten = std::min(line->len, pline->len) / 2.0f;

			if (std::min(a2pa, pa2a) <= (std::numbers::pi / 2.0f + 0.00001f)) {
				if (a2pa < pa2a) {
					float shorten = t * std::sin(a2pa / 2.0f) / std::cos(a2pa / 2.0f);
					float a = (static_cast<float>(std::numbers::pi) - a2pa) / 2.0f;

					if (shorten > maxshorten) {
						line->s0 = pline->s1 = maxshorten;
						line->tr0 = pline->tr1 = maxshorten * std::sin(a) / std::cos(a);

					} else {
						line->s0 = pline->s1 = shorten;
					}

				} else {
					float shorten = t * std::sin(pa2a / 2.0f) / std::cos(pa2a / 2.0f);
					float a = (static_cast<float>(std::numbers::pi) - pa2a) / 2.0f;

					if (shorten > maxshorten) {
						line->s0  = pline->s1 = maxshorten;
						line->tl0 = pline->tl1 = maxshorten * std::sin(a) / std::cos(a);

					} else {
						line->s0 = pline->s1 = shorten;
					}
				}

			} else {
				line->has_prev = false;
			}
		}

		if (!line->has_prev) {
			pline->has_next = false;
		}
	}

	// compute line geometry
	for (size_t i = 0; i < nlines; i++) {
		Line* line = &lines[i];

		// shorten lines if needed
		line->x0 = line->x0 + line->s0 * line->cos_a;
		line->y0 = line->y0 + line->s0 * line->sin_a;
		line->x1 = line->x1 - line->s1 * line->cos_a;
		line->y1 = line->y1 - line->s1 * line->sin_a;

		// compute initial values for left, right, leftcenter, rightcenter points
		line->xl0 = line->x0 + line->tl0 * line->sin_a;
		line->yl0 = line->y0 - line->tl0 * line->cos_a;
		line->xr0 = line->x0 - line->tr0 * line->sin_a;
		line->yr0 = line->y0 + line->tr0 * line->cos_a;
		line->xl1 = line->x1 + line->tl1 * line->sin_a;
		line->yl1 = line->y1 - line->tl1 * line->cos_a;
		line->xr1 = line->x1 - line->tr1 * line->sin_a;
		line->yr1 = line->y1 + line->tr1 * line->cos_a;

		// compute tips
		line->xlt0 = line->xl0 - t * line->cos_a;
		line->ylt0 = line->yl0 - t * line->sin_a;
		line->xrt0 = line->xr0 - t * line->cos_a;
		line->yrt0 = line->yr0 - t * line->sin_a;
		line->xlt1 = line->xl1 + t * line->cos_a;
		line->ylt1 = line->yl1 + t * line->sin_a;
		line->xrt1 = line->xr1 + t * line->cos_a;
		line->yrt1 = line->yr1 + t * line->sin_a;
	}

	// draw the lines
	for (size_t i = 0; i < nlines; i++) {
		Line* line = &lines[i];
		Line* pline = &lines[(nlines + i - 1) % nlines];

		if (line->has_prev) {
			float pa2a = normalizeAngle(pline->a - line->a);
			float a2pa = normalizeAngle(line->a - pline->a);

			if (a2pa < pa2a) {
				drawFan(line->xr0, line->yr0, pline->a, line->a, line->tl0 + line->tr0, static_cast<float>(halfBrushSize) + (line->tr0 / t * static_cast<float>(halfBrushSize)), 0, color);

			} else {
				drawFan(line->xl0, line->yl0, pline->a, line->a, line->tl0 + line->tr0, static_cast<float>(halfBrushSize) - (line->tl0 / t * static_cast<float>(halfBrushSize)), static_cast<float>(brushSize), color);
			}
		}

		float tl0 = static_cast<float>(halfBrushSize) - (line->tl0 / t) * static_cast<float>(halfBrushSize);
		float tl1 = static_cast<float>(halfBrushSize) - (line->tl1 / t) * static_cast<float>(halfBrushSize);

		float tr0 = static_cast<float>(halfBrushSize) + (line->tr0 / t) * static_cast<float>(halfBrushSize);
		float tr1 = static_cast<float>(halfBrushSize) + (line->tr1 / t) * static_cast<float>(halfBrushSize);

		addVertex(line->xr0, line->yr0, tr0, static_cast<float>(halfBrushSize), color);
		addVertex(line->xr1, line->yr1, tr1, static_cast<float>(halfBrushSize), color);
		addVertex(line->xl1, line->yl1, tl1, static_cast<float>(halfBrushSize), color);
		addVertex(line->xl0, line->yl0, tl0, static_cast<float>(halfBrushSize), color);
		addVertex(line->xr0, line->yr0, tr0, static_cast<float>(halfBrushSize), color);
		addVertex(line->xl1, line->yl1, tl1, static_cast<float>(halfBrushSize), color);

		if (!line->has_prev) {
			addVertex(line->xl0, line->yl0, tl0, static_cast<float>(halfBrushSize), color);
			addVertex(line->xlt0, line->ylt0, tl0, 0.0f, color);
			addVertex(line->xr0, line->yr0, tr0, static_cast<float>(halfBrushSize), color);
			addVertex(line->xr0, line->yr0, tr0, static_cast<float>(halfBrushSize), color);
			addVertex(line->xlt0, line->ylt0, tl0, 0.0f, color);
			addVertex(line->xrt0, line->yrt0, tr0, 0.0, color);
		}

		if (!line->has_next) {
			addVertex(line->xlt1, line->ylt1, tl1, 0.0f, color);
			addVertex(line->xl1, line->yl1, tl1, static_cast<float>(halfBrushSize), color);
			addVertex(line->xr1, line->yr1, tr1, static_cast<float>(halfBrushSize), color);
			addVertex(line->xlt1, line->ylt1, tl1, 0.0f, color);
			addVertex(line->xr1, line->yr1, tr1, static_cast<float>(halfBrushSize), color);
			addVertex(line->xrt1, line->yrt1, tr1, 0.0f, color);
		}
	}

	points.clear();
	delete [] lines;
}


//
//	OtOscilloscope::drawFan
//

void OtOscilloscope::drawFan(float _cx, float _cy, float _pa, float _a, float _t, float _s, float _e, uint32_t color) {
	int nsteps;
	float* angles;

	float pa2a = normalizeAngle(_a - _pa);
	float a2pa = normalizeAngle(_pa - _a);

	if (a2pa < pa2a) {
		_t = -_t;
		nsteps = (int) std::max(1.0, std::round(a2pa / (std::numbers::pi / 8.0)));
		angles = (float*) alloca(sizeof(float) * (nsteps + 1));

		for (auto i = 0; i <= nsteps; i++) {
			angles[i] = _a + i * a2pa / nsteps;
		}

	} else {
		nsteps = (int) std::max(1.0, std::round(pa2a / (std::numbers::pi / 8.0)));
		angles = (float*) alloca(sizeof(float) * (nsteps + 1));

		for (auto i = 0; i <= nsteps; i++) {
			angles[i] = _pa + i * pa2a / nsteps;
		}
	}

	for (auto i = 1; i <= nsteps; i++) {
		addVertex(_cx + _t * std::sin(angles[i - 1]), _cy - _t * std::cos(angles[i - 1]), _e, static_cast<float>(halfBrushSize), color);
		addVertex(_cx, _cy, _s, static_cast<float>(halfBrushSize), color);
		addVertex(_cx + _t * std::sin(angles[i]), _cy - _t * std::cos(angles[i]), _e, static_cast<float>(halfBrushSize), color);
	}
}
