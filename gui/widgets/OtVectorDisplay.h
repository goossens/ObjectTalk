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
	OtObject setDecay(int steps, float staty, float value);

	inline float getScale() { return scale; }

	inline OtObject setHorizontalAlignment(OtUiAlignment alignment) {
		horizontalAlign = alignment;
		return OtWidget(this);
	}

	inline OtObject setVerticalAlignment(OtUiAlignment alignment) {
		verticalAlign = alignment;
		return OtWidget(this);
	}

	// style releated methods
	OtObject setColor(const std::string& color);
	OtObject setAlpha(float alpha);
	OtObject setWidth(float width);
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

	// get type definition
	static OtType getMeta();

private:
	// properties
	float width = 0.0f;
	float height = 0.0f;
	float scale = 1.0f;
	bool autoScale = false;
	OtUiAlignment horizontalAlign = OtUiAlignLeft;
	OtUiAlignment verticalAlign = OtUiAlignTop;

	// oscilloscope to render display
	OtOscilloscope scope;

	// style variables
	struct Style {
		uint32_t color = 0xffffffff;
		float width = 1.0f;
		bool centeredText = false;
	};

	Style style;
	std::vector<Style> styles;

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
};
