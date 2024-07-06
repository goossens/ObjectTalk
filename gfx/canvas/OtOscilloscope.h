//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "OtBlit.h"
#include "OtBlur.h"
#include "OtColor.h"
#include "OtDynamicVertexBuffer.h"
#include "OtFrameBuffer.h"
#include "OtSampler.h"
#include "OtShaderProgram.h"
#include "OtTexture.h"
#include "OtUniformVec4.h"
#include "OtVertex.h"


//
//	OtOscilloscope
//

class OtOscilloscope {
public:
	// update properties
	inline void setSize(int w, int h) {
		width = w;
		height = h;
	}

	inline void setBrightness(float b) { brightness = b; }
	inline void setTopLeftOrigin() { origin = topLeftOrigin; }
	inline void setBottomLeftOrigin() { origin = bottomLeftOrigin; }
	inline void setCenterOrigin() { origin = centerOrigin; }

	inline void setDecay(int s, float ds, float dv) {
		decaySteps = s;
		decayStart = ds;
		decayValue = dv;
	}

	// high level draw functions
	void drawLine(float x0, float y0, float x1, float y1, float lineWidth, uint32_t color);
	void drawRectangle(float x, float y, float w, float h, float lineWidth, uint32_t color);
	void drawCircle(float x, float y, float radius, float steps, float lineWidth, uint32_t color);
	void drawSevenSegment(float x, float y, float size, const std::string& text, float lineWidth, uint32_t color);
	void drawText(float x, float y, float size, const std::string& text, float lineWidth, uint32_t color);

	// render the vector display
	int render();

private:
	// properties
	int width = 100;
	int height = 100;
	float brightness = 1.0f;

	int decaySteps = 4;
	float decayStart = 0.1f;
	float decayValue = 0.8f;

	enum {
		topLeftOrigin,
		bottomLeftOrigin,
		centerOrigin
	} origin = topLeftOrigin;

	// draw a series of connected line segments
	void beginDraw(float x, float y);
	void drawTo(float x, float y);
	void endDraw(float lineWidth, uint32_t color);

	// rendering variable
	std::vector<glm::vec2> points;
	std::vector<OtVertexPosUvCol2D> vertices;

	inline void addVertex(float x, float y, float u, float v, uint32_t color) {
		vertices.emplace_back(
			glm::vec2(x, y),
			glm::vec2(u / brushSize, 1.0f - v / brushSize),
			color);
	}

	void drawFan(float _cx, float _cy, float _pa, float _a, float _t, float _s, float _e, uint32_t color);

	std::vector<OtDynamicVertexBuffer> vertexBuffers;
	std::vector<uint32_t> vertexBufferSizes;
	int currentDrawStep = 0;

	// filters
	OtBlur blur;
	OtBlit blit;

	// GPU resources
	OtFrameBuffer texture{OtTexture::rgba8Texture};
	OtFrameBuffer blur1{OtTexture::rgba8Texture};
	OtFrameBuffer blur2{OtTexture::rgba8Texture};
	OtUniformVec4 uniform = OtUniformVec4("u_params", 1);
	OtSampler sampler = OtSampler("s_texture");
	OtShaderProgram shader{"OtOscilloscopeVS", "OtOscilloscopeFS"};

	OtTexture brush;
	static constexpr int brushSize = 64;
	static constexpr int halfBrushSize = brushSize / 2;
};
