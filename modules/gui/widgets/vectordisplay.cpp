//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/numbers.h"

#include "application.h"
#include "color.h"
#include "simplexfont.h"
#include "vectordisplay.h"
#include "vectordisplayshader.h"


//
//	globals
//

static const bgfx::EmbeddedShader embeddedShaders[] = {
	BGFX_EMBEDDED_SHADER(vs_vectordisplay),
	BGFX_EMBEDDED_SHADER(fs_vectordisplay),
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
		bgfx::createEmbeddedShader(embeddedShaders, type, "vs_vectordisplay"),
		bgfx::createEmbeddedShader(embeddedShaders, type, "fs_vectordisplay"),
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

	// set initial number of decay steps
	setDecay(4, 0.1, 0.8);

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
	OT_DEBUG(OtFormat("updateFrameBuffers %d %d", bufferWidth, bufferHeight));
	const uint64_t flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP;
	frameBuffer0 = bgfx::createFrameBuffer(bufferWidth, bufferHeight, bgfx::TextureFormat::RGBA16F, flags);
	frameBuffer1 = bgfx::createFrameBuffer(glowWidth, glowHeight, bgfx::TextureFormat::RGBA16F, flags);
	frameBuffer2 = bgfx::createFrameBuffer(glowWidth, glowHeight, bgfx::TextureFormat::RGBA16F, flags);
}


//
//	OtVectorDisplayClass::beginDraw
//

void OtVectorDisplayClass::beginDraw(float x, float y) {
	glm::vec2 point;
	point.x = x * drawScale + drawOffsetX;
	point.y = y * drawScale + drawOffsetY;
	points.push_back(point);
}


//
//	OtVectorDisplayClass::drawTo
//

void OtVectorDisplayClass::drawTo(float x, float y) {
	glm::vec2 point;
	point.x = x * drawScale + drawOffsetX;
	point.y = y * drawScale + drawOffsetY;
	points.push_back(point);
}


//
//	OtVectorDisplayClass::addVertex
//

void OtVectorDisplayClass::addVertex(float x, float y, float u, float v) {
	Vertex vertex;
	vertex.position = glm::vec3(x, y, 0.0);
	vertex.uv = glm::vec2(u / TEXTURE_SIZE, 1.0f - v / TEXTURE_SIZE);
	vertex.color = color;
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
		float x0, y0, x1, y1;                     // nominal points
		float a;                                  // angle
		float sin_a, cos_a;                       // precomputed trig

		float xl0, yl0, xl1, yl1;                 // left side of the box
		float xr0, yr0, xr1, yr1;                 // right side of the box

		bool is_first, is_last;
		bool has_next, has_prev;                  // booleans indicating whether this line connects to prev/next

		float xlt0, ylt0, xlt1, ylt1;             // coordinates of endcaps (if !has_prev/!has_next)
		float xrt0, yrt0, xrt1, yrt1;             // coordinates of endcaps (if !has_prev/!has_next)

		float tl0, tl1, tr0, tr1;

		float s0, s1;                             // shorten line by this amount

		float len;
	};

	// from the list of points, build a list of lines
	size_t nlines = points.size() - 1;
	Line* lines = new Line[nlines];

	float t = std::max((0.01 * (bufferWidth + bufferHeight) / 2.0) * thickness * drawScale / 2.0, 6.0);
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
		line->a     = std::atan2(line->y1 - line->y0, line->x1 - line->x0); // angle from positive x axis, increasing ccw, [-pi, pi]
		line->sin_a = std::sin(line->a);
		line->cos_a = std::cos(line->a);
		line->len   = std::sqrt( (line->x1 - line->x0) * (line->x1 - line->x0) + (line->y1 - line->y0) * (line->y1 - line->y0) );

		// figure out what connections we have
		line->has_prev = (!line->is_first || (line->is_first && firstIsLast));
		line->has_next = (!line->is_last || (line->is_last && firstIsLast));

		// initialize thicknesses/shortens to default values
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
//	OtVectorDisplayClass::setDecay
//

void OtVectorDisplayClass::setDecay(int s, float i, float d) {
	decaySteps = s;
	decayStart = i;
	decayValue = d;

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
//	OtVectorDisplayClass::setBrightness
//

void OtVectorDisplayClass::setBrightness(float b) {
	brightness = b;
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
//	OtVectorDisplayClass::drawText
//

void OtVectorDisplayClass::drawText(float x, float y, float scale, const std::string& text) {
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
					beginDraw(x + vx * scale, y - vy * scale);
					isDrawing = true;

				} else {
					drawTo(x + vx * scale, y - vy * scale);
				}
			}

			if (isDrawing) {
				endDraw();
			}

			x += chr[1] * scale;
		}
	}
}


//
//	OtVectorDisplayClass::setTransform
//

void OtVectorDisplayClass::setTransform(float offsetX, float offsetY, float scale) {
	drawOffsetX = offsetX;
	drawOffsetY = offsetY;
	drawScale   = scale;
}


//
//	OtVectorDisplayClass::setColor
//

void OtVectorDisplayClass::setColor(const std::string& c) {
	color = OtColorParseToUint32(c);
}


//
//	OtVectorDisplayClass::setThickness
//

void OtVectorDisplayClass::setThickness(float t) {
	thickness = t;
}


//
//	OtVectorDisplayClass::addLine
//

int OtVectorDisplayClass::addLine(float x0, float y0, float x1, float y1) {
	Shape shape;
	shape.id = nextShapeID++;
	shape.type = Shape::lineType;
	shape.thickness = thickness;
	shape.color = color;
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
	shape.type = Shape::rectangleType;
	shape.thickness = thickness;
	shape.color = color;
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
	shape.type = Shape::circleType;
	shape.thickness = thickness;
	shape.color = color;
	shape.x = x;
	shape.y = y;
	shape.radius = radius;
	shape.steps = steps;
	shapes.push_back(shape);
	return shape.id;
}


//
//	OtVectorDisplayClass::addText
//

int OtVectorDisplayClass::addText(float x, float y, float scale, const std::string& text) {
	Shape shape;
	shape.id = nextShapeID++;
	shape.type = Shape::textType;
	shape.thickness = thickness;
	shape.color = color;
	shape.x = x;
	shape.y = y;
	shape.scale = scale;
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
//	OtVectorDisplayClass::updateText
//

void OtVectorDisplayClass::updateText(int id, float x, float y, float scale, const std::string& text) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x = x;
			shape.y = y;
			shape.radius = scale;
			shape.text = text;
			return;
		}
	}
}


//
//	OtVectorDisplayClass::updateThickness
//

void OtVectorDisplayClass::updateThickness(int id, float thickness) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.thickness = thickness;
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
	// render all shapes
	for (auto& shape : shapes) {
		thickness = shape.thickness;
		color = shape.color;

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

			case Shape::textType:
				drawText(shape.x, shape.y, shape.scale, shape.text);
				break;
		}
	}

	// get frame size
	int fw = OtApplicationClass::getWidth();
	int fh = OtApplicationClass::getHeight();

	// determine vector display dimensions
	int vx = x < 0 ? fw + (x * fw) / 100 : (x * fw) / 100;
	int vy = y < 0 ? fh + (y * fh) / 100 : (y * fh) / 100;
	int vw = (w * fw) / 100;
	int vh = (h * fh) / 100;

	// update frame buffers if required
	if (vw != bufferWidth || vh != bufferHeight) {
		bufferWidth = vw;
		bufferHeight = vh;
		glowWidth = vw / 3;
		glowHeight = vh / 3;
		updateFrameBuffers();
	}

	// setup BGFX view
	int view = OtApplicationClass::getNextViewID();
	bgfx::setViewClear(view, BGFX_CLEAR_COLOR);
	bgfx::setViewRect(view, 0.0, 0.0, vw, vh);
	bgfx::setViewFrameBuffer(view, frameBuffer0);

	glm::mat4 matrix = glm::ortho(0.0f, (float) vw, (float) vh, 0.0f, -1.0f, 1.0f);
	bgfx::setViewTransform(view, nullptr, glm::value_ptr(matrix));

	// advance to next draw step
	currentDrawStep = (currentDrawStep + 1) % decaySteps;

	// update the vertex buffer
	bgfx::update(vertexBuffers[currentDrawStep], 0, bgfx::copy(vertices.data(), vertices.size() * sizeof(Vertex)));
	vertexBuffersSize[currentDrawStep] = vertices.size();

	for (auto c = 0; c < decaySteps; c++) {
		int stepi = decaySteps - c - 1;
		int i = (currentDrawStep + decaySteps - stepi) % decaySteps;

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
			bgfx::setVertexBuffer(0, vertexBuffers[i], 0, vertexBuffersSize[i]); // explicitly feed vertex number!

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

	if (ImGui::SliderInt("Decay Steps", &decaySteps, 1, 60)) {
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
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtVectorDisplayClass>("VectorDisplay", OtScreenObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtVectorDisplayClass::init));

		type->set("setBrightness", OtFunctionClass::create(&OtVectorDisplayClass::setBrightness));
		type->set("setDecay", OtFunctionClass::create(&OtVectorDisplayClass::setDecay));

		type->set("setThickness", OtFunctionClass::create(&OtVectorDisplayClass::setThickness));
		type->set("setColor", OtFunctionClass::create(&OtVectorDisplayClass::setColor));

		type->set("addLine", OtFunctionClass::create(&OtVectorDisplayClass::addLine));
		type->set("addRectangle", OtFunctionClass::create(&OtVectorDisplayClass::addRectangle));
		type->set("addCircle", OtFunctionClass::create(&OtVectorDisplayClass::addCircle));
		type->set("addText", OtFunctionClass::create(&OtVectorDisplayClass::addText));

		type->set("updateLine", OtFunctionClass::create(&OtVectorDisplayClass::updateLine));
		type->set("updateRectangle", OtFunctionClass::create(&OtVectorDisplayClass::updateRectangle));
		type->set("updateCircle", OtFunctionClass::create(&OtVectorDisplayClass::updateCircle));
		type->set("updateText", OtFunctionClass::create(&OtVectorDisplayClass::updateText));

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
