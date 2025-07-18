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

#include "OtFrameBuffer.h"
#include "OtOscilloscope.h"
#include "OtUi.h"

#include "OtWidget.h"


//
//	OtVectorDisplayClass
//

class OtVectorDisplayClass;
using OtVectorDisplay = OtObjectPointer<OtVectorDisplayClass>;

class OtVectorDisplayClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	OtObject setSize(int width, int height);
	OtObject setScale(float scale);
	OtObject setAutoScale();
	OtObject setBrightness(float brightness);
	OtObject setTopLeftOrigin() ;
	OtObject setBottomLeftOrigin();
	OtObject setCenterOrigin();
	OtObject setDecay(int steps, float start, float value);

	inline float getScale() { return scale; }

	inline OtObject setHorizontalAlignment(OtUi::Alignment alignment) {
		horizontalAlign = alignment;
		return OtWidget(this);
	}

	inline OtObject setVerticalAlignment(OtUi::Alignment alignment) {
		verticalAlign = alignment;
		return OtWidget(this);
	}

	// style related methods
	OtObject setColor(const std::string& color);
	OtObject setAlpha(float alpha);
	OtObject setWidth(float width);

	void pushStyle();
	void popStyle();

	// determine sizes
	float getSevenSegmentWidth(const std::string& text, float size);
	float getTextWidth(const std::string& text, float size);

	// manage shapes
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

	void clear();

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtVectorDisplayClass>;
	OtVectorDisplayClass() = default;

private:
	// properties
	float width = 0.0f;
	float height = 0.0f;
	float scale = 1.0f;
	bool autoScale = false;
	OtUi::Alignment horizontalAlign = OtUi::Alignment::left;
	OtUi::Alignment verticalAlign = OtUi::Alignment::top;

	// oscilloscope to render display
	OtOscilloscope scope;
	OtFrameBuffer framebuffer{OtTexture::rgba8Texture};

	// style variables
	struct Style {
		uint32_t color = 0xffffffff;
		float width = 1.0f;
	};

	Style style;
	std::vector<Style> styles;

	// shapes that make up the display
	struct Shape {
		enum class Type {
			line,
			rectangle,
			circle,
			sevenSegment,
			text
		};

		int id;
		bool enabled;
		Type type;
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
		};

		std::string text;
	};

	std::vector<Shape> shapes;
	int nextShapeID = 1;
};
