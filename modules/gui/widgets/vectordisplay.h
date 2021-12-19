//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "controller.h"
#include "blit.h"
#include "blur.h"
#include "screenobject.h"


//
//	OtVectorDisplayClass
//

class OtVectorDisplayClass;
typedef std::shared_ptr<OtVectorDisplayClass> OtVectorDisplay;

class OtVectorDisplayClass : public OtScreenObjectClass {
public:
	// constructor/destructor
	OtVectorDisplayClass();
	~OtVectorDisplayClass();

	// initialize vector display
	void init(int _x, int _y, int _w, int _h) { x = _x; y = _y; w = _w; h = _h; }

	// display properties
	void setBrightness(float brightness);
	void setDecay(int steps, float decayStart, float decay);

	// shape API
	void setTransform(float offsetX, float offsetY, float scale);
	void setColor(const std::string& color);
	void setAlpha(float alpha);
	void setThickness(float thickness);

	float getTextWidth(const std::string& text, float scale);
	float getTextHeight(const std::string& text, float scale);

	int addLine(float x0, float y0, float x1, float y1);
	int addRectangle(float x, float y, float w, float h);
	int addCircle(float x, float y, float radius, float steps);
	int addText(float x, float y, float scale, const std::string& text);

	void updateLine(int id, float x0, float y0, float x1, float y1);
	void updateRectangle(int id, float x, float y, float w, float h);
	void updateCircle(int id, float x, float y, float radius, float steps);
	void updateText(int id, float x, float y, float scale, const std::string& text);

	void updateThickness(int id, float thickness);
	void updateColor(int id, const std::string& color);
	void updateAlpha(int id, float alpha);

	void enableShape(int id);
	void disableShape(int id);
	void deleteShape(int id);

	// render content
	void render();

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtVectorDisplay create();

private:
	// draw a series of connected line segments
	void beginDraw(float x, float y);
	void drawTo(float x, float y);
	void endDraw();

	// high level draw functions
	void drawLine(float x0, float y0, float x1, float y1);
	void drawRectangle(float x, float y, float w, float h);
	void drawCircle(float x, float y, float radius, float steps);
	void drawText(float x, float y, float scale, const std::string& text);

	// vector display dimensions (in % of screen)
	int x = 0;
	int y = 0;
	int w = 100;
	int h = 100;

	// BGFX shader
	bgfx::UniformHandle paramsUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform = BGFX_INVALID_HANDLE;
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;

	// buffers
	int bufferWidth = -1;
	int bufferHeight = -1;
	int glowWidth = -1;
	int glowHeight = -1;
	bgfx::FrameBufferHandle frameBuffer0 = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle frameBuffer1 = BGFX_INVALID_HANDLE;
	bgfx::FrameBufferHandle frameBuffer2 = BGFX_INVALID_HANDLE;

	std::vector<bgfx::DynamicVertexBufferHandle> vertexBuffers;
	std::vector<int> vertexBuffersSize;

	// texture to draw lines
	bgfx::TextureHandle lineTexture;

	// create/update frame buffers
	void updateFrameBuffers();

	// vector display vertex definition
	struct Vertex {
		glm::vec3 position;
		glm::vec2 uv;
		uint32_t color;

		static bgfx::VertexLayout getLayout() {
			static bool ready = false;
			static bgfx::VertexLayout layout;

			if (!ready) {
				layout.begin()
					.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
					.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
					.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
					.end();

				ready = true;
			}

			return layout;
		}
	};

	std::vector<Vertex> vertices;
	void addVertex(float x, float y, float u, float v);
	void drawFan(float _cx, float _cy, float _pa, float _a, float _t, float _s, float _e);

	// list of points (used while drawing lines)
	std::vector<glm::vec2> points;

	// shpaes that make up display
	struct Shape {
		enum {
			lineType,
			rectangleType,
			circleType,
			textType
		};

		int id;
		bool enabled;
		int type;
		float thickness;
		uint32_t color;

		union {
			float x;
			float x0;
		};

		union {
			float y;
			float y0;
		};

		union {
			float x1;
			float w;
			float radius;
			float scale;
		};

		union {
			float y1;
			float h;
			float steps;
		};

		std::string text;
	};

	std::vector<Shape> shapes;
	int nextShapeID = 1;

	// rendering properties
	uint32_t color = 0xffffffff;
	float thickness = 1.0;

	int decaySteps = 0;
	int currentDrawStep = 0;
	float decayStart = 0.04;
	float decayValue = 0.8;

	float brightness = 1.0;
	float drawScale = 1.0;
	float drawOffsetX = 0.0;
	float drawOffsetY = 0.0;

	// filters
	OtBlur blur;
	OtBlit blit;
};


//
//	Controller widget
//

OT_CONTROLLER(VectorDisplay)
