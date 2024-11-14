//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtColorParser.h"
#include "OtSimplexFont.h"
#include "OtUi.h"

#include "OtVectorDisplay.h"


//
//	OtVectorDisplayClass::init
//

void OtVectorDisplayClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 5:
			setVerticalAlignment(OtUi::Alignment(parameters[4]->operator int()));

		case 4:
			setHorizontalAlignment(OtUi::Alignment(parameters[3]->operator int()));

		case 3:
			setScale(parameters[2]->operator float());

		case 2:
			setSize(parameters[0]->operator int(), parameters[1]->operator int());

		case 0:
			break;

		default:
			OtLogFatal("[VectorDisplay] constructor expects 0, 2, 3, 4 or 5 arguments (not {})", count);
	}
}


//
//	OtVectorDisplayClass::setSize
//

OtObject OtVectorDisplayClass::setSize(int w, int h) {
	width = w;
	height = h;
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setScale
//

OtObject OtVectorDisplayClass::setScale(float s) {
	scale = s;
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setAutoScale
//

OtObject OtVectorDisplayClass::setAutoScale() {
	autoScale = true;
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setBrightness
//

OtObject OtVectorDisplayClass::setBrightness(float brightness) {
	scope.setBrightness(brightness);
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setDecay
//

OtObject OtVectorDisplayClass::setDecay(int steps, float start, float value) {
	scope.setDecay(steps, start, value);
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setTopLeftOrigin
//

OtObject OtVectorDisplayClass::setTopLeftOrigin() {
	scope.setTopLeftOrigin();
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setBottomLeftOrigin
//

OtObject OtVectorDisplayClass::setBottomLeftOrigin() {
	scope.setBottomLeftOrigin();
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setCenterOrigin
//

OtObject OtVectorDisplayClass::setCenterOrigin() {
	scope.setCenterOrigin();
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setColor
//

OtObject OtVectorDisplayClass::setColor(const std::string& color) {
	style.color = OtColorParser::toUint32(color);
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setAlpha
//

OtObject OtVectorDisplayClass::setAlpha(float alpha) {
	int a = alpha * 255;
	style.color = (style.color & 0xffffff) | a << 24;
	return OtWidget(this);
}


//
//	OtVectorDisplayClass::setWidth
//

OtObject OtVectorDisplayClass::setWidth(float width) {
	style.width = width;
	return OtWidget(this);
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
//	OtVectorDisplayClass::getSevenSegmentWidth
//

float OtVectorDisplayClass::getSevenSegmentWidth(const std::string& text, float size) {
	return (text.size() * 0.8f * size - 0.3f * size) * scale;
}


//
//	OtVectorDisplayClass::getTextWidth
//

float OtVectorDisplayClass::getTextWidth(const std::string& text, float size) {
	auto factor = size / 32.0f;
	float width = 0.0;

	for (auto& c : text) {
		if (c >= 32 || c <= 126) {
			const int8_t* chr = simplex[c - 32];
			width += (float) chr[1] * factor;
		}
	}

	return width * scale;
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
//	OtVectorDisplayClass::addCenteredRectangle
//

int OtVectorDisplayClass::addCenteredRectangle(float x, float y, float w, float h) {
	Shape shape;
	shape.id = nextShapeID++;
	shape.enabled = true;
	shape.type = Shape::rectangleType;
	shape.width = style.width;
	shape.color = style.color;
	shape.x = x - w / 2.0;
	shape.y = y - h / 2.0;
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
//	OtVectorDisplayClass::updateCenteredRectangle
//

void OtVectorDisplayClass::updateCenteredRectangle(int id, float x, float y, float w, float h) {
	for (auto& shape : shapes) {
		if (shape.id == id) {
			shape.x = x - w / 2.0;
			shape.y = y - h / 2.0;
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
			shape.color = OtColorParser::toUint32(color);
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
//	OtVectorDisplayClass::clear
//

void OtVectorDisplayClass::clear() {
	shapes.clear();
}


//
//	OtVectorDisplayClass::render
//

void OtVectorDisplayClass::render() {
	// handle auto scaling
	if (autoScale) {
		auto available = ImGui::GetContentRegionAvail();
		scale = std::min(available.x / float(width), available.y / float(height));
	}

	// calculate size of scope
	ImVec2 size{width * scale, height * scale};
	scope.setSize(size.x, size.y);

	// render all shapes
	for (auto& shape : shapes) {
		if (shape.enabled) {
			switch (shape.type) {
				case Shape::lineType:
					scope.drawLine(
						shape.x0 * scale, shape.y0 * scale,
						shape.x1 * scale, shape.y1 * scale,
						shape.width, shape.color);
					break;

				case Shape::rectangleType:
					scope.drawRectangle(
						shape.x * scale, shape.y * scale,
						shape.w * scale, shape.h  * scale,
						shape.width, shape.color);
					break;

				case Shape::circleType:
					scope.drawCircle(
						shape.x * scale, shape.y * scale,
						shape.radius * scale, shape.steps,
						shape.width, shape.color);
					break;

				case Shape::sevenSegmentType:
					scope.drawSevenSegment(
						shape.x * scale, shape.y * scale,
						shape.size * scale,
						shape.text,
						shape.width, shape.color);
					break;

				case Shape::textType:
					scope.drawText(
						shape.x * scale,
						shape.y * scale,
						shape.size * scale,
						shape.text,
						shape.width, shape.color);
					break;
			}
		}
	}

	// render scope frame, align it and put it on the screen
	auto index = scope.render();
	OtUi::align(size, horizontalAlign, verticalAlign);
	ImGui::Image((ImTextureID)(intptr_t) index, size);
}


//
//	OtVectorDisplayClass::getMeta
//

OtType OtVectorDisplayClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtVectorDisplayClass>("VectorDisplay", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtVectorDisplayClass::init));
		type->set("setSize", OtFunction::create(&OtVectorDisplayClass::setSize));
		type->set("setScale", OtFunction::create(&OtVectorDisplayClass::setScale));
		type->set("getScale", OtFunction::create(&OtVectorDisplayClass::getScale));
		type->set("setAutoScale", OtFunction::create(&OtVectorDisplayClass::setAutoScale));
		type->set("setBrightness", OtFunction::create(&OtVectorDisplayClass::setBrightness));
		type->set("setTopLeftOrigin", OtFunction::create(&OtVectorDisplayClass::setTopLeftOrigin));
		type->set("setBottomLeftOrigin", OtFunction::create(&OtVectorDisplayClass::setBottomLeftOrigin));
		type->set("setCenterOrigin", OtFunction::create(&OtVectorDisplayClass::setCenterOrigin));
		type->set("setHorizontalAlignment", OtFunction::create(&OtVectorDisplayClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtVectorDisplayClass::setVerticalAlignment));
		type->set("setDecay", OtFunction::create(&OtVectorDisplayClass::setDecay));

		type->set("setColor", OtFunction::create(&OtVectorDisplayClass::setColor));
		type->set("setAlpha", OtFunction::create(&OtVectorDisplayClass::setAlpha));
		type->set("setWidth", OtFunction::create(&OtVectorDisplayClass::setWidth));

		type->set("pushStyle", OtFunction::create(&OtVectorDisplayClass::pushStyle));
		type->set("popStyle", OtFunction::create(&OtVectorDisplayClass::popStyle));

		type->set("getSevenSegmentWidth", OtFunction::create(&OtVectorDisplayClass::getSevenSegmentWidth));
		type->set("getTextWidth", OtFunction::create(&OtVectorDisplayClass::getTextWidth));

		type->set("addLine", OtFunction::create(&OtVectorDisplayClass::addLine));
		type->set("addRectangle", OtFunction::create(&OtVectorDisplayClass::addRectangle));
		type->set("addCenteredRectangle", OtFunction::create(&OtVectorDisplayClass::addCenteredRectangle));
		type->set("addCircle", OtFunction::create(&OtVectorDisplayClass::addCircle));
		type->set("addSevenSegment", OtFunction::create(&OtVectorDisplayClass::addSevenSegment));
		type->set("addText", OtFunction::create(&OtVectorDisplayClass::addText));

		type->set("updateLine", OtFunction::create(&OtVectorDisplayClass::updateLine));
		type->set("updateRectangle", OtFunction::create(&OtVectorDisplayClass::updateRectangle));
		type->set("updateCenteredRectangle", OtFunction::create(&OtVectorDisplayClass::updateCenteredRectangle));
		type->set("updateCircle", OtFunction::create(&OtVectorDisplayClass::updateCircle));
		type->set("updateSevenSegment", OtFunction::create(&OtVectorDisplayClass::updateSevenSegment));
		type->set("updateSevenSegmentString", OtFunction::create(&OtVectorDisplayClass::updateSevenSegmentString));
		type->set("updateText", OtFunction::create(&OtVectorDisplayClass::updateText));
		type->set("updateTextString", OtFunction::create(&OtVectorDisplayClass::updateTextString));

		type->set("updateColor", OtFunction::create(&OtVectorDisplayClass::updateColor));
		type->set("updateAlpha", OtFunction::create(&OtVectorDisplayClass::updateAlpha));
		type->set("updateWidth", OtFunction::create(&OtVectorDisplayClass::updateWidth));

		type->set("enableShape", OtFunction::create(&OtVectorDisplayClass::enableShape));
		type->set("disableShape", OtFunction::create(&OtVectorDisplayClass::disableShape));
		type->set("deleteShape", OtFunction::create(&OtVectorDisplayClass::deleteShape));
	}

	return type;
}
