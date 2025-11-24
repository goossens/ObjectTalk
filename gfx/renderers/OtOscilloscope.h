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
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "OtBlur.h"
#include "OtCompositingAdd.h"
#include "OtRenderPipeline.h"
#include "OtTexture.h"
#include "OtVertexBuffer.h"


//
//	OtOscilloscope
//

class OtOscilloscope {
public:
	// constructor
	OtOscilloscope();

	// update properties
	inline void setBrightness(float b) { brightness = b; }
	inline void setTopLeftOrigin() { origin = Origin::topLeft; }
	inline void setBottomLeftOrigin() { origin = Origin::bottomLeft; }
	inline void setCenterOrigin() { origin = Origin::center; }

	inline void setDecay(size_t s, float ds, float dv) {
		decaySteps = s;
		decayStart = ds;
		decayValue = dv;
	}

	// high level draw functions
	void drawLine(float x0, float y0, float x1, float y1, float lineWidth, glm::vec4& color);
	void drawRectangle(float x, float y, float w, float h, float lineWidth, glm::vec4& color);
	void drawCircle(float x, float y, float radius, float steps, float lineWidth, glm::vec4& color);
	void drawSevenSegment(float x, float y, float size, const std::string& text, float lineWidth, glm::vec4& color);
	void drawText(float x, float y, float size, const std::string& text, float lineWidth, glm::vec4& color);

	// render to a specified texture
	void render(OtTexture& texture);

private:
	// properties
	int width;
	int height;
	float brightness = 1.0f;

	size_t decaySteps = 4;
	float decayStart = 0.1f;
	float decayValue = 0.8f;

	enum class Origin {
		topLeft,
		bottomLeft,
		center
	};

	Origin origin = Origin::topLeft;

	// draw a series of connected line segments
	void beginDraw(float x, float y);
	void drawTo(float x, float y);
	void endDraw(float lineWidth, glm::vec4& color);

	// rendering variable
	std::vector<glm::vec2> points;
	std::vector<OtVertexPosUvCol2D> vertices;

	inline void addVertex(float x, float y, float u, float v, glm::vec4& color) {
		vertices.emplace_back(
			glm::vec2(x, y),
			glm::vec2(u / brushSize, 1.0f - v / brushSize),
			color);
	}

	void drawFan(float _cx, float _cy, float _pa, float _a, float _t, float _s, float _e, glm::vec4& color);

	std::vector<OtVertexBuffer> vertexBuffers;
	int currentDrawStep = 0;

	// filters
	OtBlur blur;
	OtCompositingAdd compositor;

	// GPU resources
	OtTexture blur1;
	OtTexture blur2;

	OtTexture brush;
	static constexpr int brushSize = 64;
	static constexpr int halfBrushSize = brushSize / 2;

	OtRenderPipeline pipeline;
	OtSampler sampler;
};
