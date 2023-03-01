//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"

#include "OtAppObject.h"
#include "OtBlit.h"
#include "OtBlur.h"
#include "OtController.h"
#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtShader.h"
#include "OtTexture.h"
#include "OtUniformVec4.h"


//
//	OtVectorDisplayClass
//

class OtVectorDisplayClass;
using OtVectorDisplay = OtObjectPointer<OtVectorDisplayClass>;

class OtVectorDisplayClass : public OtAppObjectClass {
public:
	// constructor/destructor
	OtVectorDisplayClass();
	~OtVectorDisplayClass();

	// initialize vector display
	void init(size_t count, OtObject* parameters);

	// update properties
	OtObject setScreenArea(int x, int y, int w, int h);
	OtObject setBrightness(float brightness);
	OtObject setDecay(int steps, float decayStart, float decay);
	OtObject setTopLeftOrigin();
	OtObject setBottomLeftOrigin();
	OtObject setCenterOrigin();

	// shape API
	OtObject setColor(const std::string& color);
	OtObject setAlpha(float alpha);
	OtObject setWidth(float width);
	OtObject setTransform(float offsetX, float offsetY, float scale);
	OtObject setCenteredText();

	void pushStyle();
	void popStyle();

	float getSevenSegmentWidth(const std::string& text, float size);
	float getTextWidth(const std::string& text, float size);

	int addLine(float x0, float y0, float x1, float y1);
	int addRectangle(float x, float y, float w, float h);
	int addCenteredRectangle(float x, float y, float w, float h);
	int addCircle(float x, float y, float radius, float steps);
	int addSevenSegment(float x, float y, float size, const std::string& text);
	int addText(float x, float y, float size, const std::string& text);

	void updateLine(int id, float x0, float y0, float x1, float y1);
	void updateRectangle(int id, float x, float y, float w, float h);
	void updateCenteredRectangle(int id, float x, float y, float w, float h);
	void updateCircle(int id, float x, float y, float radius, float steps);
	void updateSevenSegment(int id, float x, float y, float size, const std::string& text);
	void updateSevenSegmentString(int id, const std::string& text);
	void updateText(int id, float x, float y, float size, const std::string& text);
	void updateTextString(int id, const std::string& text);

	void updateColor(int id, const std::string& color);
	void updateAlpha(int id, float alpha);
	void updateWidth(int id, float width);

	void enableShape(int id);
	void disableShape(int id);
	void deleteShape(int id);

	// render content
	void render() override;

	// GUI to change properties
	void renderGUI();

	// get type definition
	static OtType getMeta();

private:
	// draw a series of connected line segments
	void beginDraw(float x, float y);
	void drawTo(float x, float y);
	void endDraw();

	// high level draw functions
	void drawLine(float x0, float y0, float x1, float y1);
	void drawRectangle(float x, float y, float w, float h);
	void drawCircle(float x, float y, float radius, float steps);
	void drawSevenSegment(float x, float y, float size, const std::string& text);
	void drawText(float x, float y, float size, bool centered, const std::string& text);

	// vector display dimensions (in % of screen)
	int x = 0;
	int y = 0;
	int w = 100;
	int h = 100;

	// GPU assets
	OtUniformVec4 uniform = OtUniformVec4("u_params", 1);
	OtTexture lineTexture;
	OtSampler sampler = OtSampler("s_texture");
	OtShader shader = OtShader("OtVectorDisplayVS", "OtVectorDisplayFS");

	OtFrameBuffer frameBuffer0 = OtFrameBuffer(OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture);
	OtFrameBuffer frameBuffer1 = OtFrameBuffer(OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture);
	OtFrameBuffer frameBuffer2 = OtFrameBuffer(OtFrameBuffer::rgba8Texture, OtFrameBuffer::noTexture);

	// buffers
	int bufferWidth = -1;
	int bufferHeight = -1;
	int glowWidth = -1;
	int glowHeight = -1;

	std::vector<bgfx::DynamicVertexBufferHandle> vertexBuffers;
	std::vector<uint32_t> vertexBufferSizes;

	// create/update buffers
	void updateVertexBuffers();

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

	// shapes that make up the display
	struct Shape {
		enum {
			lineType,
			rectangleType,
			circleType,
			sevenSegmentType,
			textType
		};

		int id;
		bool enabled;
		int type;
		float width;
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
			float size;
		};

		union {
			float y1;
			float h;
			float steps;
			bool centered;
		};

		std::string text;
	};

	std::vector<Shape> shapes;
	int nextShapeID = 1;

	// rendering properties
	struct Style {
		uint32_t color = 0xffffffff;
		float width = 1.0;
		float drawOffsetX = 0.0;
		float drawOffsetY = 0.0;
		float drawScale = 1.0;
		bool centeredText = false;
	};

	Style style;
	std::vector<Style> styles;

	float brightness = 1.0;

	int decaySteps = 4;
	int currentDrawStep = 0;
	float decayStart = 0.1;
	float decayValue = 0.8;

	// rendering origin
	enum {
		topLeftOrigin,
		bottomLeftOrigin,
		centerOrigin
	};

	int origin = topLeftOrigin;

	// filters
	OtBlur blur;
	OtBlit blit;
};


//
//	Controller widget
//

OT_CONTROLLER(VectorDisplay)
