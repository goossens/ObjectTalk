//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/embedded_shader.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "imgui.h"

#include "OtFunction.h"
#include "OtNumbers.h"

#include "OtColor.h"
#include "OtFramework.h"
#include "OtSimplexFont.h"
#include "OtVectorDisplay.h"
#include "OtVectorDisplayShader.h"


//
//	globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(OtVectorDisplayVS),
	BGFX_EMBEDDED_SHADER(OtVectorDisplayFS),
	BGFX_EMBEDDED_SHADER_END()
};

const int MAX_NUMBER_VERTICES = 20000;
const int TEXTURE_SIZE = 64;
const int HALF_TEXTURE_SIZE = TEXTURE_SIZE / 2;


//
//	OtVectorDisplayClass::OtVectorDisplayClass
//

OtVectorDisplayClass::OtVectorDisplayClass() {
	// register uniforms
	paramsUniform = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, 1);
	textureUniform = bgfx::createUniform("s_texture", bgfx::UniformType::Sampler);

	// initialize shader
	bgfx::RendererType::Enum type = bgfx::getRendererType();

	shader = bgfx::createProgram(
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtVectorDisplayVS"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "OtVectorDisplayFS"),
		true);

	// create line texture
	const bgfx::Memory* mem = bgfx::alloc(TEXTURE_SIZE * TEXTURE_SIZE * 4);
	unsigned char* p = (unsigned char*) mem->data;

	for (auto y = -HALF_TEXTURE_SIZE; y < HALF_TEXTURE_SIZE; y++) {
		for (auto x = -HALF_TEXTURE_SIZE; x < HALF_TEXTURE_SIZE; x++) {
			*p++ = 0xff;
			*p++ = 0xff;
			*p++ = 0xff;
			auto distance = std::min(1.0, std::sqrt((double) (x * x + y * y)) / (double) HALF_TEXTURE_SIZE);
			*p++ = (unsigned char) (std::clamp(std::pow(16.0, -2.0 * distance), 0.0, 1.0) * 255.0);
		}
	}

	const uint32_t flags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP | BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT;
	lineTexture = bgfx::createTexture2D(TEXTURE_SIZE, TEXTURE_SIZE, false, 1, bgfx::TextureFormat::BGRA8, flags, mem);

	// create filters
	blur = std::make_shared<OtBlurClass>();
	blit = std::make_shared<OtBlitClass>();
}


//
//	OtVectorDisplayClass::~OtVectorDisplayClass
//

OtVectorDisplayClass::~OtVectorDisplayClass() {
	// release resources
	if (bgfx::isValid(frameBuffer0)) {
		bgfx::destroy(frameBuffer0);
	}

	if (bgfx::isValid(frameBuffer1)) {
		bgfx::destroy(frameBuffer1);
	}

	if (bgfx::isValid(frameBuffer2)) {
		bgfx::destroy(frameBuffer2);
	}

	for (auto& buffer : vertexBuffers) {
		bgfx::destroy(buffer);
	}

	bgfx::destroy(paramsUniform);
	bgfx::destroy(textureUniform);
	bgfx::destroy(lineTexture);
	bgfx::destroy(shader);
}


//
//	OtVectorDisplayClass::updateFrameBuffers
//

void OtVectorDisplayClass::updateFrameBuffers() {
	// release resources (if required)
	if (bgfx::isValid(frameBuffer0)) {
		bgfx::destroy(frameBuffer0);
	}

	if (bgfx::isValid(frameBuffer1)) {
		bgfx::destroy(frameBuffer1);
	}

	if (bgfx::isValid(frameBuffer2)) {
		bgfx::destroy(frameBuffer2);
	}

	// create new framebuffer
	const uint64_t flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
	frameBuffer0 = bgfx::createFrameBuffer(bufferWidth, bufferHeight, bgfx::TextureFormat::BGRA8, flags);
	frameBuffer1 = bgfx::createFrameBuffer(glowWidth, glowHeight, bgfx::TextureFormat::BGRA8, flags);
	frameBuffer2 = bgfx::createFrameBuffer(glowWidth, glowHeight, bgfx::TextureFormat::BGRA8, flags);
}


//
//	OtVectorDisplayClass::updateVertexBuffers
//

void OtVectorDisplayClass::updateVertexBuffers() {
	if (vertexBuffers.size() != 0) {
		for (auto& buffer : vertexBuffers) {
			bgfx::destroy(buffer);
		}

		vertexBuffers.clear();
		vertexBuffersSize.clear();
	}

	for (size_t i = 0; i < decaySteps; i++) {
		vertexBuffers.push_back(bgfx::createDynamicVertexBuffer(MAX_NUMBER_VERTICES, Vertex::getLayout()));
		vertexBuffersSize.push_back(0);
	}
}


//
//	OtVectorDisplayClass::beginDraw
//

void OtVectorDisplayClass::beginDraw(float x, float y) {
	glm::vec2 point;
	point.x = x * style.drawScale + style.drawOffsetX;
	point.y = y * style.drawScale + style.drawOffsetY;
	points.push_back(point);
}


//
//	OtVectorDisplayClass::drawTo
//

void OtVectorDisplayClass::drawTo(float x, float y) {
	glm::vec2 point;
	point.x = x * style.drawScale + style.drawOffsetX;
	point.y = y * style.drawScale + style.drawOffsetY;
	points.push_back(point);
}


//
//	OtVectorDisplayClass::addVertex
//

void OtVectorDisplayClass::addVertex(float x, float y, float u, float v) {
	Vertex vertex;
	vertex.position = glm::vec3(x, y, 0.0);
	vertex.uv = glm::vec2(u / TEXTURE_SIZE, 1.0f - v / TEXTURE_SIZE);
	vertex.color = style.color;
	vertices.push_back(vertex);
}


//
//	normalizeAngle
//

inline float normalizeAngle(float angle) {
	float wrap = 2.0 * std::numbers::pi;
	float result = std::fmod(angle, wrap);
	return result < 0.0 ? result + wrap : result;
}


//
//	OtVectorDisplayClass::drawFan
//

void OtVectorDisplayClass::drawFan(float _cx, float _cy, float _pa, float _a, float _t, float _s, float _e) {
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
		addVertex(_cx + _t * std::sin(angles[i - 1]), _cy - _t * std::cos(angles[i - 1]), _e, (float) HALF_TEXTURE_SIZE);
		addVertex(_cx, _cy, _s, (float) HALF_TEXTURE_SIZE);
		addVertex(_cx + _t * std::sin(angles[i]), _cy - _t * std::cos(angles[i]), _e, (float) HALF_TEXTURE_SIZE);
	}
}


//
//	OtVectorDisplayClass::endDraw
//

void OtVectorDisplayClass::endDraw() {
	if (points.size() < 2) {
		OtExcept("Invalid number of points in VectorDisplay draw action");
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

	// from the list of points, build a list of lines
	size_t nlines = points.size() - 1;
	Line* lines = new Line[nlines];

	float t = std::max((0.01 * (bufferWidth + bufferHeight) / 2.0) * style.width / 2.0, 6.0);
	bool firstIsLast = std::abs(points[0].x - points[nlines].x) < 0.1 && std::abs(points[0].y - points[nlines].y) < 0.1;

	// compute basics
	for (auto i = 1; i < points.size(); i++)
	{
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
		line->s0 = line->s1 = 0.0;
	}

	// compute adjustments for connected line segments
	for (auto i = 0; i < nlines; i++)
	{
		Line* line = &lines[i];
		Line* pline = &lines[(nlines + i - 1) % nlines];

		if (line->has_prev) {
			float pa2a = normalizeAngle(pline->a - line->a);
			float a2pa = normalizeAngle(line->a - pline->a);
			float maxshorten = std::min(line->len, pline->len) / 2.0f;

			if (std::min(a2pa, pa2a) <= (std::numbers::pi / 2.0 + 0.00001)) {
				if (a2pa < pa2a) {
					float shorten = t * std::sin(a2pa / 2.0f) / std::cos(a2pa / 2.0f);
					float a = (std::numbers::pi - a2pa) / 2.0f;

					if (shorten > maxshorten) {
						line->s0 = pline->s1 = maxshorten;
						line->tr0 = pline->tr1 = maxshorten * std::sin(a) / std::cos(a);

					} else {
						line->s0 = pline->s1 = shorten;
					}

				} else {
					float shorten = t * std::sin(pa2a / 2.0f) / std::cos(pa2a / 2.0f);
					float a = (std::numbers::pi - pa2a) / 2.0f;

					if (shorten > maxshorten) {
						line->s0  = pline->s1 = maxshorten;
						line->tl0 = pline->tl1 = maxshorten * std::sin(a) / std::cos(a);

					} else {
						line->s0 = pline->s1 = shorten;
					}
				}

			} else {
				line->has_prev = 0;
			}
		}

		if (!line->has_prev) {
			pline->has_next = 0;
		}
	}

	// compute line geometry
	for (auto i = 0; i < nlines; i++) {
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
	for (auto i = 0; i < nlines; i++) {
		Line* line = &lines[i];
		Line* pline = &lines[(nlines + i - 1) % nlines];

		if (line->has_prev) {
			float pa2a = normalizeAngle(pline->a - line->a);
			float a2pa = normalizeAngle(line->a - pline->a);

			if (a2pa < pa2a) {
				drawFan(line->xr0, line->yr0, pline->a, line->a, line->tl0 + line->tr0, (float) HALF_TEXTURE_SIZE + (line->tr0 / t * (float) HALF_TEXTURE_SIZE), 0);

			} else {
				drawFan(line->xl0, line->yl0, pline->a, line->a, line->tl0 + line->tr0, (float) HALF_TEXTURE_SIZE - (line->tl0 / t * (float) HALF_TEXTURE_SIZE), (float)TEXTURE_SIZE);
			}
		}

		float tl0 = (float) HALF_TEXTURE_SIZE - (line->tl0 / t) * (float) HALF_TEXTURE_SIZE;
		float tl1 = (float) HALF_TEXTURE_SIZE - (line->tl1 / t) * (float) HALF_TEXTURE_SIZE;

		float tr0 = (float) HALF_TEXTURE_SIZE + (line->tr0 / t) * (float) HALF_TEXTURE_SIZE;
		float tr1 = (float) HALF_TEXTURE_SIZE + (line->tr1 / t) * (float) HALF_TEXTURE_SIZE;

		addVertex(line->xr0, line->yr0, tr0, (float) HALF_TEXTURE_SIZE);
		addVertex(line->xr1, line->yr1, tr1, (float) HALF_TEXTURE_SIZE);
		addVertex(line->xl1, line->yl1, tl1, (float) HALF_TEXTURE_SIZE);
		addVertex(line->xl0, line->yl0, tl0, (float) HALF_TEXTURE_SIZE);
		addVertex(line->xr0, line->yr0, tr0, (float) HALF_TEXTURE_SIZE);
		addVertex(line->xl1, line->yl1, tl1, (float) HALF_TEXTURE_SIZE);

		if (!line->has_prev) {
			addVertex(line->xl0, line->yl0, tl0, (float) HALF_TEXTURE_SIZE);
			addVertex(line->xlt0, line->ylt0, tl0, 0.0f);
			addVertex(line->xr0, line->yr0, tr0, (float) HALF_TEXTURE_SIZE);
			addVertex(line->xr0, line->yr0, tr0, (float) HALF_TEXTURE_SIZE);
			addVertex(line->xlt0, line->ylt0, tl0, 0.0f);
			addVertex(line->xrt0, line->yrt0, tr0, 0.0f);
		}

		if (!line->has_next) {
			addVertex(line->xlt1, line->ylt1, tl1, 0.0f);
			addVertex(line->xl1, line->yl1, tl1, (float) HALF_TEXTURE_SIZE);
			addVertex(line->xr1, line->yr1, tr1, (float) HALF_TEXTURE_SIZE);
			addVertex(line->xlt1, line->ylt1, tl1, 0.0f);
			addVertex(line->xr1, line->yr1, tr1, (float) HALF_TEXTURE_SIZE);
			addVertex(line->xrt1, line->yrt1, tr1, 0.0f);
		}
	}

	points.clear();
	delete [] lines;
}


//
//	OtVectorDisplayClass::setBrightness
//

OtObject OtVectorDisplayClass::setBrightness(float b) {
	brightness = b;
	return shared();
}


//
//	OtVectorDisplayClass::setDecay
//

OtObject OtVectorDisplayClass::setDecay(int s, float i, float d) {
	decaySteps = s;
	decayStart = i;
	decayValue = d;
	return shared();
}


//
//	OtVectorDisplayClass::setTopLeftOrigin
//

OtObject OtVectorDisplayClass::setTopLeftOrigin() {
	origin = topLeftOrigin;
	return shared();
}


//
//	OtVectorDisplayClass::setBottomLeftOrigin
//

OtObject OtVectorDisplayClass::setBottomLeftOrigin() {
	origin = bottomLeftOrigin;
	return shared();
}


//
//	OtVectorDisplayClass::setCenterOrigin
//

OtObject OtVectorDisplayClass::setCenterOrigin() {
	origin = centerOrigin;
	return shared();
}


//
//	OtVectorDisplayClass::setColor
//

OtObject OtVectorDisplayClass::setColor(const std::string& color) {
	style.color = OtColorParseToUint32(color);
	return shared();
}


//
//	OtVectorDisplayClass::setAlpha
//

OtObject OtVectorDisplayClass::setAlpha(float alpha) {
	int a = alpha * 255;
	style.color = (style.color & 0xffffff) | a << 24;
	return shared();
}


//
//	OtVectorDisplayClass::setWidth
//

OtObject OtVectorDisplayClass::setWidth(float width) {
	style.width = width;
	return shared();
}


//
//	OtVectorDisplayClass::setTransform
//

OtObject OtVectorDisplayClass::setTransform(float offsetX, float offsetY, float scale) {
	style.drawOffsetX = offsetX;
	style.drawOffsetY = offsetY;
	style.drawScale   = scale;
	return shared();
}


//
//	OtVectorDisplayClass::pushStyle
//

void OtVectorDisplayClass::pushStyle() {
	styles.push_back(style);
}


//
//	OtVectorDisplayClass::popStyle
//

void OtVectorDisplayClass::popStyle() {
	style = styles.back();
	styles.pop_back();
}


//
//	OtVectorDisplayClass::drawLine
//

void OtVectorDisplayClass::drawLine(float x0, float y0, float x1, float y1) {
	beginDraw(x0, y0);
	drawTo(x1, y1);
	endDraw();
}


//
//	OtVectorDisplayClass::drawRectangle
//

void OtVectorDisplayClass::drawRectangle(float x, float y, float w, float h) {
	beginDraw(x, y);
	drawTo(x + w, y);
	drawTo(x + w, y + h);
	drawTo(x, y + h);
	drawTo(x, y);
	endDraw();
}


//
//	OtVectorDisplayClass::drawCircle
//

void OtVectorDisplayClass::drawCircle(float x, float y, float radius, float steps) {
	beginDraw(x, y - radius);

	float step = std::numbers::pi * 2.0 / steps;

	for (float angle = step; angle < 2.0 * std::numbers::pi - 0.001; angle += step) {
		drawTo(x + radius * std::sin(angle), y - radius * std::cos(angle));
	}

	drawTo(x, y - radius);
	endDraw();
}


//
//	OtVectorDisplayClass::drawSevenSegment
//

void OtVectorDisplayClass::drawSevenSegment(float x, float y, float size, const std::string& text) {
	// calculate scaling
	auto s = size / 100.0;
	auto t = origin == topLeftOrigin ? s : -s;

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
			beginDraw(x + 2.7 * s, y + 0.0);
			drawTo(x + 49.0 * s, y + 0.0);
			drawTo(x + 49.0 * s, y + 0.5 * t);
			drawTo(x + 41.0 * s, y + 8.0 * t);
			drawTo(x + 9.8 * s, y + 8.0 * t);
			drawTo(x + 2.2 * s, y + 0.5 * t);
			drawTo(x + 2.7 * s, y + 0.0);
			endDraw();
		}

		// segment B
		if (pattern & 0b0000010) {
			beginDraw(x + 50.0 * s, y + 2.7 * t);
			drawTo(x + 50.0 * s, y + 49.0 * t);
			drawTo(x + 46.0 * s, y + 49.0 * t);
			drawTo(x + 42.0 * s, y + 45.0 * t);
			drawTo(x + 42.0 * s, y + 9.8 * t);
			drawTo(x + 50.0 * s, y + 2.2 * t);
			drawTo(x + 50.0 * s, y + 2.7 * t);
			endDraw();
		}

		// segment C
		if (pattern & 0b0000100) {
			beginDraw(x + 50.0 * s, y + 98.0 * t);
			drawTo(x + 42.0 * s, y + 91.0 * t);
			drawTo(x + 42.0 * s, y + 55.0 * t);
			drawTo(x + 46.0 * s, y + 51.0 * t);
			drawTo(x + 50.0 * s, y + 51.0 * t);
			drawTo(x + 50.0 * s, y + 98.0 * t);
			endDraw();
		}

		// segment D
		if (pattern & 0b0001000) {
			beginDraw(x + 2.2 * s, y + 100.0 * t);
			drawTo(x + 9.8 * s, y + 92.0 * t);
			drawTo(x + 41.0 * s, y + 92.0 * t);
			drawTo(x + 49.0 * s, y + 100.0 * t);
			drawTo(x + 2.7 * s, y + 100.0 * t);
			drawTo(x + 2.2 * s, y + 100.0 * t);
			endDraw();
		}

		// segment E
		if (pattern & 0b0010000) {
			beginDraw(x + 0.0, y + 98.0 * t);
			drawTo(x + 0.0, y + 51.0 * t);
			drawTo(x + 4.0 * s, y + 51.0 * t);
			drawTo(x + 8.0 * s, y + 55.0 * t);
			drawTo(x + 8.0 * s, y + 91.0 * t);
			drawTo(x + 0.5 * s, y + 98.0 * t);
			drawTo(x + 0.0, y + 98.0 * t);
			endDraw();
		}

		// segment F
		if (pattern & 0b0100000) {
			beginDraw(x + 0.5 * s, y + 2.2 * t);
			drawTo(x + 8.0 * s, y + 9.8 * t);
			drawTo(x + 8.0 * s, y + 45.0 * t);
			drawTo(x + 4.0 * s, y + 49.0 * t);
			drawTo(x + 0.0 * s, y + 49.0 * t);
			drawTo(x + 0.0 * s, y + 2.7 * t);
			drawTo(x + 0.5 * s, y + 2.2 * t);
			endDraw();
		}

		// segment G
		if (pattern & 0b1000000) {
			beginDraw(x + 10.2 * s, y + 46.0 * t);
			drawTo(x + 40.0 * s, y + 46.0 * t);
			drawTo(x + 44.0 * s, y + 50.0 * t);
			drawTo(x + 40.0 * s, y + 54.0 * t);
			drawTo(x + 10.2 * s, y + 54.0 * t);
			drawTo(x + 6.2 * s, y + 50.0 * t);
			drawTo(x + 10.2 * s, y + 46.0 * t);
			endDraw();
		}

		x += size * 0.8;
	}
}


//
//	OtVectorDisplayClass::drawText
//

void OtVectorDisplayClass::drawText(float x, float y, float size, const std::string& text) {
	auto scaleX = size / 32.0;
	auto scaleY = scaleX * (origin == topLeftOrigin ? -1.0 : 1.0);

	for (auto& c : text) {
		if (c >= 32 || c <= 126) {
			const int8_t* chr = simplex[c - 32];
			const int8_t* p = chr + 2;
			bool isDrawing = false;

			for (auto i = 0; i < chr[0]; i++) {
				int vx = *p++;
				int vy = *p++;

				if (vx == -1 && vy == -1) {
					if (isDrawing) {
						endDraw();
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
				endDraw();
			}

			x += chr[1] * scaleX;
		}
	}
}


//
//	OtVectorDisplayClass::getSevenSegmentWidth
//

float OtVectorDisplayClass::getSevenSegmentWidth(const std::string& text, float size) {
	return text.size() * 0.8 * size - 0.3 * size;
}


//
//	OtVectorDisplayClass::getTextWidth
//

float OtVectorDisplayClass::getTextWidth(const std::string& text, float size) {
	auto scale = size / 32.0;
	float width = 0.0;

	for (auto& c : text) {
		if (c >= 32 || c <= 126) {
			const int8_t* chr = simplex[c - 32];
			width += (float) chr[1] * scale;
		}
	}

	return width;
}


//
//	OtVectorDisplayClass::addLine
//

int OtVectorDisplayClass::addLine(float x0, float y0, float x1, float y1) {
	Shape shape;
	shape.id = nextShapeID++;
	shape.enabled = true;
	shape.type = Shape::lineType;
	shape.width = style.width;
	shape.color = style.color;
	shape.x0 = x0;
	shape.y0 = y0;
	shape.x1 = x1;
	shape.y1 = y1;
	shapes.push_back(shape);
	return shape.id;
}


//
//	OtVectorDisplayClass::addRectangle
//

int OtVectorDisplayClass::addRectangle(float x, float y, float w, float h) {
	Shape shape;
	shape.id = nextShapeID++;
	shape.enabled = true;
	shape.type = Shape::rectangleType;
	shape.width = style.width;
	shape.color = style.color;
	shape.x = x;
	shape.y = y;
	shape.w = w;
	shape.h = h;
	shapes.push_back(shape);
	return shape.id;
}


//
//	OtVectorDisplayClass::addCircle
//

int OtVectorDisplayClass::addCircle(float x, float y, float radius, float steps) {
	Shape shape;
	shape.id = nextShapeID++;
	shape.enabled = true;
	shape.type = Shape::circleType;
	shape.width = style.width;
	shape.color = style.color;
	shape.x = x;
	shape.y = y;
	shape.radius = radius;
	shape.steps = steps;
	shapes.push_back(shape);
	return shape.id;
}


//
//	OtVectorDisplayClass::addSevenSegment
//

int OtVectorDisplayClass::addSevenSegment(float x, float y, float size, const std::string& text) {
	Shape shape;
	shape.enabled = true;
	shape.id = nextShapeID++;
	shape.type = Shape::sevenSegmentType;
	shape.width = style.width;
	shape.color = style.color;
	shape.x = x;
	shape.y = y;
	shape.size = size;
	shape.text = text;
	shapes.push_back(shape);
	return shape.id;
}


//
//	OtVectorDisplayClass::addText
//

int OtVectorDisplayClass::addText(float x, float y, float size, const std::string& text) {
	Shape shape;
	shape.enabled = true;
	shape.id = nextShapeID++;
	shape.type = Shape::textType;
	shape.width = style.width;
	shape.color = style.color;
	shape.x = x;
	shape.y = y;
	shape.size = size;
	shape.text = text;
	shapes.push_back(shape);
	return shape.id;
}


//
//	OtVectorDisplayClass::updateLine
//

void OtVectorDisplayClass::updateLine(int id, float x0, float y0, float x1, float y1) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x0 = x0;
			shape.y0 = y0;
			shape.x1 = x1;
			shape.y1 = y1;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateRectangle
//

void OtVectorDisplayClass::updateRectangle(int id, float x, float y, float w, float h) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x = x;
			shape.y = y;
			shape.w = w;
			shape.h = h;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateCircle
//

void OtVectorDisplayClass::updateCircle(int id, float x, float y, float radius, float steps) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x = x;
			shape.y = y;
			shape.radius = radius;
			shape.steps = steps;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateSevenSegment
//

void OtVectorDisplayClass::updateSevenSegment(int id, float x, float y, float size, const std::string& text) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x = x;
			shape.y = y;
			shape.size = size;
			shape.text = text;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateSevenSegmentString
//

void OtVectorDisplayClass::updateSevenSegmentString(int id, const std::string& text) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.text = text;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateText
//

void OtVectorDisplayClass::updateText(int id, float x, float y, float size, const std::string& text) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x = x;
			shape.y = y;
			shape.size = size;
			shape.text = text;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateTextString
//

void OtVectorDisplayClass::updateTextString(int id, const std::string& text) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.text = text;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateWidth
//

void OtVectorDisplayClass::updateWidth(int id, float width) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.width = width;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateColor
//

void OtVectorDisplayClass::updateColor(int id, const std::string& color) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.color = OtColorParseToUint32(color);
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateAlpha
//

void OtVectorDisplayClass::updateAlpha(int id, float alpha) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			int a = alpha * 255;
			shape.color = (shape.color & 0xffffff) | a << 24;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::enableShape
//

void OtVectorDisplayClass::enableShape(int id) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.enabled = true;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::disableShape
//

void OtVectorDisplayClass::disableShape(int id) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.enabled = false;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::deleteShape
//

void OtVectorDisplayClass::deleteShape(int id) {
	for (auto it = shapes.begin(); it < shapes.end(); it++) {
		if (it->id == id) {
			shapes.erase(it);
			return;
		}
	}
}



//
//	OtVectorDisplayClass::render
//

void OtVectorDisplayClass::render() {
	// update vertexBuffers if required
	if (decaySteps != vertexBuffers.size()) {
		updateVertexBuffers();
	}

	// render all shapes
	pushStyle();

	for (auto& shape : shapes) {
		if (shape.enabled) {
			style.width = shape.width;
			style.color = shape.color;

			switch (shape.type) {
				case Shape::lineType:
					drawLine(shape.x0, shape.y0, shape.x1, shape.y1);
					break;

				case Shape::rectangleType:
					drawRectangle(shape.x, shape.y, shape.w, shape.h);
					break;

				case Shape::circleType:
					drawCircle(shape.x, shape.y, shape.radius, shape.steps);
					break;

				case Shape::sevenSegmentType:
					drawSevenSegment(shape.x, shape.y, shape.size, shape.text);
					break;

				case Shape::textType:
					drawText(shape.x, shape.y, shape.size, shape.text);
					break;
			}
		}
	}

	popStyle();

	// get screen size
	OtFramework framework = OtFrameworkClass::instance();
	int sw = framework->getWidth();
	int sh = framework->getHeight();

	// determine vector display dimensions
	int vx = x < 0 ? sw + (x * sw) / 100 : (x * sw) / 100;
	int vy = y < 0 ? sh + (y * sh) / 100 : (y * sh) / 100;
	int vw = (w * sw) / 100;
	int vh = (h * sh) / 100;

	// update frame buffers if required
	if (vw != bufferWidth || vh != bufferHeight) {
		bufferWidth = vw;
		bufferHeight = vh;
		glowWidth = vw / 3;
		glowHeight = vh / 3;
		updateFrameBuffers();
	}

	// setup BGFX view
	int view = OtFrameworkClass::instance()->getNextViewID();
	bgfx::setViewClear(view, BGFX_CLEAR_COLOR);
	bgfx::setViewRect(view, 0.0, 0.0, vw, vh);
	bgfx::setViewFrameBuffer(view, frameBuffer0);

	glm::mat4 matrix;

	switch (origin) {
		case topLeftOrigin:
			matrix = glm::ortho(0.0f, (float) vw, (float) vh, 0.0f, -1.0f, 1.0f);
			break;

		case bottomLeftOrigin:
			matrix = glm::ortho(0.0f, (float) vw, 0.0f, (float) vh, -1.0f, 1.0f);
			break;

		case centerOrigin:
			matrix = glm::ortho((float) (-vw / 2), (float) (vw / 2), (float) (-vh / 2), (float) (vh / 2), -1.0f, 1.0f);
			break;
	}

	bgfx::setViewTransform(view, nullptr, glm::value_ptr(matrix));

	// advance to next draw step
	currentDrawStep = (currentDrawStep + 1) % decaySteps;

	// update the vertex buffer
	if (vertices.size()) {
		bgfx::update(vertexBuffers[currentDrawStep], 0, bgfx::copy(vertices.data(), vertices.size() * sizeof(Vertex)));
	}

	vertexBuffersSize[currentDrawStep] = vertices.size();

	// render all decay steps
	for (auto c = 0; c < decaySteps; c++) {
		int stepi = decaySteps - c - 1;
		int i = (currentDrawStep + decaySteps - stepi) % decaySteps;

		// don't render empty buffers
		if (vertexBuffersSize[i] != 0) {
			float alpha;

			if (stepi == 0) {
				alpha = 1.0;

			} else if (stepi == 1) {
				alpha = decayStart;

			} else {
				alpha = std::pow(decayValue, stepi - 1.0f) * decayStart;
			}

			glm::vec4 params = glm::vec4(alpha, 0.0, 0.0, 0.0);
			bgfx::setUniform(paramsUniform, &params);
			bgfx::setTexture(0, textureUniform, lineTexture);
			bgfx::setVertexBuffer(0, vertexBuffers[i], 0, vertexBuffersSize[i]);

			bgfx::setState(
				BGFX_STATE_WRITE_RGB |
				BGFX_STATE_WRITE_A |
				BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_DST_ALPHA) |
				BGFX_STATE_BLEND_EQUATION_SEPARATE(BGFX_STATE_BLEND_EQUATION_ADD, BGFX_STATE_BLEND_EQUATION_MAX));

			bgfx::submit(view, shader);
		}
	}

	vertices.clear();

	// apply glow
	blur->setIntensity(1.05 + ((brightness - 1.0) / 5.0));
	blur->setAlpha(1.0);

	for (auto p = 0; p < 4; p++) {
		// run horizontal blur
		blur->setHorizontalScale(1.0 / glowWidth);
		blur->setVerticalScale(0.0);

		if (p == 0) {
			blur->render(glowWidth, glowHeight, bgfx::getTexture(frameBuffer0), frameBuffer1);

		} else {
			blur->render(glowWidth, glowHeight, bgfx::getTexture(frameBuffer2), frameBuffer1);
		}

		// run vertical blur
		blur->setHorizontalScale(0.0);
		blur->setVerticalScale(1.0 / glowHeight);
		blur->render(glowWidth, glowHeight, bgfx::getTexture(frameBuffer1), frameBuffer2);
	}

	// combine original rendering with glow
	blit->setIntensity(1.0);
	blit->setAlpha(1.0);

	blit->setState(
		BGFX_STATE_WRITE_RGB |
		BGFX_STATE_WRITE_A |
		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE));

	// render original
	blit->render(vx, vy, vw, vh, bgfx::getTexture(frameBuffer0));

	// render glow
	blit->setIntensity(1.25 + ((brightness - 1.0) / 2.0));
	blit->render(vx, vy, vw, vh, bgfx::getTexture(frameBuffer2));
}


//
//	OtVectorDisplayClass::renderGUI
//

void OtVectorDisplayClass::renderGUI() {
	ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);

	bool dirty = false;

	if (ImGui::SliderInt("Decay Steps", &decaySteps, 1, 10)) {
		dirty = true;
	}

	ImGui::SliderFloat("Decay Start", &decayStart, 0.01f, 0.2f);
	ImGui::SliderFloat("Decay Value", &decayValue, 0.5f, 3.0f);

	if (dirty) {
		setDecay(decaySteps, decayStart, decayValue);
	}
}


//
//	OtVectorDisplayClass::getMeta
//

OtType OtVectorDisplayClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtVectorDisplayClass>("VectorDisplay", OtAppObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtVectorDisplayClass::init));

		type->set("setBrightness", OtFunctionClass::create(&OtVectorDisplayClass::setBrightness));
		type->set("setDecay", OtFunctionClass::create(&OtVectorDisplayClass::setDecay));
		type->set("setTopLeftOrigin", OtFunctionClass::create(&OtVectorDisplayClass::setTopLeftOrigin));
		type->set("setBottomLeftOrigin", OtFunctionClass::create(&OtVectorDisplayClass::setBottomLeftOrigin));
		type->set("setCenterOrigin", OtFunctionClass::create(&OtVectorDisplayClass::setCenterOrigin));

		type->set("setColor", OtFunctionClass::create(&OtVectorDisplayClass::setColor));
		type->set("setAlpha", OtFunctionClass::create(&OtVectorDisplayClass::setAlpha));
		type->set("setWidth", OtFunctionClass::create(&OtVectorDisplayClass::setWidth));
		type->set("setTransform", OtFunctionClass::create(&OtVectorDisplayClass::setTransform));

		type->set("pushStyle", OtFunctionClass::create(&OtVectorDisplayClass::pushStyle));
		type->set("popStyle", OtFunctionClass::create(&OtVectorDisplayClass::popStyle));

		type->set("getSevenSegmentWidth", OtFunctionClass::create(&OtVectorDisplayClass::getSevenSegmentWidth));
		type->set("getTextWidth", OtFunctionClass::create(&OtVectorDisplayClass::getTextWidth));

		type->set("addLine", OtFunctionClass::create(&OtVectorDisplayClass::addLine));
		type->set("addRectangle", OtFunctionClass::create(&OtVectorDisplayClass::addRectangle));
		type->set("addCircle", OtFunctionClass::create(&OtVectorDisplayClass::addCircle));
		type->set("addSevenSegment", OtFunctionClass::create(&OtVectorDisplayClass::addSevenSegment));
		type->set("addText", OtFunctionClass::create(&OtVectorDisplayClass::addText));

		type->set("updateLine", OtFunctionClass::create(&OtVectorDisplayClass::updateLine));
		type->set("updateRectangle", OtFunctionClass::create(&OtVectorDisplayClass::updateRectangle));
		type->set("updateCircle", OtFunctionClass::create(&OtVectorDisplayClass::updateCircle));
		type->set("updateSevenSegment", OtFunctionClass::create(&OtVectorDisplayClass::updateSevenSegment));
		type->set("updateSevenSegmentString", OtFunctionClass::create(&OtVectorDisplayClass::updateSevenSegmentString));
		type->set("updateText", OtFunctionClass::create(&OtVectorDisplayClass::updateText));
		type->set("updateTextString", OtFunctionClass::create(&OtVectorDisplayClass::updateTextString));

		type->set("updateColor", OtFunctionClass::create(&OtVectorDisplayClass::updateColor));
		type->set("updateAlpha", OtFunctionClass::create(&OtVectorDisplayClass::updateAlpha));
		type->set("updateWidth", OtFunctionClass::create(&OtVectorDisplayClass::updateWidth));

		type->set("enableShape", OtFunctionClass::create(&OtVectorDisplayClass::enableShape));
		type->set("disableShape", OtFunctionClass::create(&OtVectorDisplayClass::disableShape));
		type->set("deleteShape", OtFunctionClass::create(&OtVectorDisplayClass::deleteShape));
	}

	return type;
}


//
//	OtVectorDisplayClass::create
//

OtVectorDisplay OtVectorDisplayClass::create() {
	OtVectorDisplay display = std::make_shared<OtVectorDisplayClass>();
	display->setType(getMeta());
	return display;
}
