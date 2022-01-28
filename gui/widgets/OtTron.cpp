//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <ctime>

#include "OtFormat.h"
#include "OtFramework.h"
#include "OtFunction.h"
#include "OtNumbers.h"

#include "OtTron.h"


//
//	Constants
//

#define MAX_RADIUS 512.0


//
//	Render a 7 segment LED
//

static void OtLed7(ImDrawList* list, float x, float y, float size, char ch, ImU32 color) {
	// get segment pattern
	int pattern = 0;

	switch (ch) {
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
	}

	// calculate scaling
	auto s = size / 100.0;

	// segment A
	if (pattern & 0b0000001) {
		list->PathLineTo(ImVec2(x + 2.7 * s, y + 0.0));
		list->PathLineTo(ImVec2(x + 49.0 * s, y + 0.0));
		list->PathLineTo(ImVec2(x + 49.0 * s, y + 0.5 * s));
		list->PathLineTo(ImVec2(x + 41.0 * s, y + 8.0 * s));
		list->PathLineTo(ImVec2(x + 9.8 * s, y + 8.0 * s));
		list->PathLineTo(ImVec2(x + 2.2 * s, y + 0.5 * s));
		list->PathFillConvex(color);
	}

	// segment B
	if (pattern & 0b0000010) {
		list->PathLineTo(ImVec2(x + 50.0 * s, y + 2.7 * s));
		list->PathLineTo(ImVec2(x + 50.0 * s, y + 49.0 * s));
		list->PathLineTo(ImVec2(x + 46.0 * s, y + 49.0 * s));
		list->PathLineTo(ImVec2(x + 42.0 * s, y + 45.0 * s));
		list->PathLineTo(ImVec2(x + 42.0 * s, y + 9.8 * s));
		list->PathLineTo(ImVec2(x + 50.0 * s, y + 2.2 * s));
		list->PathFillConvex(color);
	}

	// segment C
	if (pattern & 0b0000100) {
		list->PathLineTo(ImVec2(x + 50.0 * s, y + 98.0 * s));
		list->PathLineTo(ImVec2(x + 42.0 * s, y + 91.0 * s));
		list->PathLineTo(ImVec2(x + 42.0 * s, y + 55.0 * s));
		list->PathLineTo(ImVec2(x + 46.0 * s, y + 51.0 * s));
		list->PathLineTo(ImVec2(x + 50.0 * s, y + 51.0 * s));
		list->PathFillConvex(color);
	}

	// segment D
	if (pattern & 0b0001000) {
		list->PathLineTo(ImVec2(x + 2.2 * s, y + 100.0 * s));
		list->PathLineTo(ImVec2(x + 9.8 * s, y + 92.0 * s));
		list->PathLineTo(ImVec2(x + 41.0 * s, y + 92.0 * s));
		list->PathLineTo(ImVec2(x + 49.0 * s, y + 100.0 * s));
		list->PathLineTo(ImVec2(x + 2.7 * s, y + 100.0 * s));
		list->PathFillConvex(color);
	}

	// segment E
	if (pattern & 0b0010000) {
		list->PathLineTo(ImVec2(x + 0.0, y + 98.0 * s));
		list->PathLineTo(ImVec2(x + 0.0, y + 51.0 * s));
		list->PathLineTo(ImVec2(x + 4.0 * s, y + 51.0 * s));
		list->PathLineTo(ImVec2(x + 8.0 * s, y + 55.0 * s));
		list->PathLineTo(ImVec2(x + 8.0 * s, y + 91.0 * s));
		list->PathLineTo(ImVec2(x + 0.5 * s, y + 98.0 * s));
		list->PathFillConvex(color);
	}

	// segment F
	if (pattern & 0b0100000) {
		list->PathLineTo(ImVec2(x + 0.5 * s, y + 2.2 * s));
		list->PathLineTo(ImVec2(x + 8.0 * s, y + 9.8 * s));
		list->PathLineTo(ImVec2(x + 8.0 * s, y + 45.0 * s));
		list->PathLineTo(ImVec2(x + 4.0 * s, y + 49.0 * s));
		list->PathLineTo(ImVec2(x + 0.0 * s, y + 49.0 * s));
		list->PathLineTo(ImVec2(x + 0.0 * s, y + 2.7 * s));
		list->PathFillConvex(color);
	}

	// segment G
	if (pattern & 0b1000000) {
		list->PathLineTo(ImVec2(x + 10.2 * s, y + 46.0 * s));
		list->PathLineTo(ImVec2(x + 40.0 * s, y + 46.0 * s));
		list->PathLineTo(ImVec2(x + 44.0 * s, y + 50.0 * s));
		list->PathLineTo(ImVec2(x + 40.0 * s, y + 54.0 * s));
		list->PathLineTo(ImVec2(x + 10.2 * s, y + 54.0 * s));
		list->PathLineTo(ImVec2(x + 6.2 * s, y + 50.0 * s));
		list->PathFillConvex(color);
	}
}


//
//	OtTronClass::update
//

void OtTronClass::update() {
	// get current frame rate
	auto framerate = OtFrameworkClass::instance()->getFrameRate();

	// update internal rotation
	tronRotation += 0.360 / framerate;

	// create new cycle if required
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);

	if (now->tm_sec == 59 && !closing && !opening) {
		closing = true;

	} else if (closing) {
		// handle closing annimation
		if (tronScale < 0.1) {
			tronScale = 0.1;
			opening = true;
			closing = false;
			createArcs();

		} else {
			tronScale -= 0.98 / framerate;
		}
	}

	else if (opening) {
		// handle opening animation
		if (tronScale > 0.99) {
			tronScale = 1.0;
			opening = false;

		} else {
			tronScale += 0.98 / framerate;
		}
	}
}


//
//	OtTronClass::render
//

void OtTronClass::render() {
	ImGui::PushID((void*) this);

	auto region = ImGui::GetContentRegionAvail();
	auto size = std::min(region.x, region.y);
	auto scale = tronScale * size / MAX_RADIUS / 2.0;

	auto pos = ImGui::GetCursorScreenPos();
	auto center = ImVec2(pos.x + region.x / 2, pos.y + size / 2);

	ImGui::InvisibleButton("", ImVec2(size, size), 0);
	ImDrawList* list = ImGui::GetWindowDrawList();

	for (auto i = arcs.begin(); i != arcs.end(); i++) {
		auto rotation = (i->rotation + tronRotation) * (i->clockwise ? 1.0 : -1.0);
		list->PathArcTo(center, i->radius * scale, i->start + rotation, i->end + rotation);
		list->PathStroke(i->color, 0, i->width * scale);
	}

	// get time of day
	std::time_t time = std::time(0);
	std::tm* now = std::localtime(&time);

	// render seconds indicator
	for (auto c = 0; c < 60; c += 1) {
		auto angle = c * std::numbers::pi * 2.0 / 60.0;
		auto inner = 240 * scale;
		auto outer = 280 * scale;

		list->AddLine(
			ImVec2(center.x + inner * sin(angle), center.y + inner * -cos(angle)),
			ImVec2(center.x + outer * sin(angle), center.y + outer * -cos(angle)),
			(c == now->tm_sec) ? color100 : color20,
			15 * scale);
	}

	// render hours and minutes
	OtLed7(list, center.x - 200 * scale, center.y - 80 * scale, 160 * scale, '8', color20);
	OtLed7(list, center.x - 200 * scale, center.y - 80 * scale, 160 * scale, '0' + now->tm_hour / 12, color100);
	OtLed7(list, center.x - 100 * scale, center.y - 80 * scale, 160 * scale, '8', color20);
	OtLed7(list, center.x - 100 * scale, center.y - 80 * scale, 160 * scale, '0' + now->tm_hour % 12, color100);
	list->AddCircleFilled(ImVec2(center.x, center.y - 30 * scale), 10 * scale, color100);
	list->AddCircleFilled(ImVec2(center.x, center.y + 30 * scale), 10 * scale, color100);
	OtLed7(list, center.x + 20 * scale, center.y - 80 * scale, 160 * scale, '8', color20);
	OtLed7(list, center.x + 20 * scale, center.y - 80 * scale, 160 * scale, '0' + now->tm_min / 10, color100);
	OtLed7(list, center.x + 120 * scale, center.y - 80 * scale, 160 * scale, '8', color20);
	OtLed7(list, center.x + 120 * scale, center.y - 80 * scale, 160 * scale, '0' + now->tm_min % 10, color100);

	ImGui::PopID();
}


//
//	OtTron::createArcs
//

void OtTronClass::createArcs() {
	// clear all previous arcs
	arcs.clear();

	// generate all arcs
	OtTronArc arc;
	bool nextCircle = false;

	while (arc.radius < MAX_RADIUS - 15) {
		// determine start of arc
		arc.start = arc.end + OtRandom(0.1, 0.3);

		// ensure arc is still meaningful
		if (arc.start > std::numbers::pi * 2.0 - 0.2) {
			nextCircle = true;

		} else {
			// determine end of arc
			arc.end = arc.start + OtRandom(0.3, std::numbers::pi / 3.0);

			// ensure arc is still meaningful
			if (arc.end > std::numbers::pi * 2.0) {
				arc.end = std::numbers::pi * 2.0 - 0.1;
				nextCircle = true;
			}

			// assign random color to arc
			arc.color = blend(color20, color100, OtRandom());

			// add arc to list
			arcs.push_back(arc);
		}

		// start next circle if required
		if (nextCircle) {
			// update angles
			arc.end = 0.0;
			arc.width = OtRandom(6.0, 14.0);
			arc.rotation = OtRandom(std::numbers::pi);
			arc.radius += arc.width + OtRandom(8.0, 12.0);
			arc.clockwise = OtRandom() > 0.5;
			nextCircle = false;
		}
	}
}


//
//	OtTronClass::blend
//

ImU32 OtTronClass::blend(ImU32 color1, ImU32 color2, float ratio) {
	auto c1 = ImGui::ColorConvertU32ToFloat4(color1);
	auto c2 = ImGui::ColorConvertU32ToFloat4(color2);

	return ImGui::ColorConvertFloat4ToU32(ImVec4(
		c1.x + (c2.x - c1.x) * ratio,
		c1.y + (c2.y - c1.y) * ratio,
		c1.z + (c2.z - c1.z) * ratio,
		c1.w + (c2.w - c1.w) * ratio));
}


//
//	OtTronClass::getMeta
//

OtType OtTronClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtTronClass>("Tron", OtWidgetClass::getMeta());
	}

	return type;
}


//
//	OtTronClass::create
//

OtTron OtTronClass::create() {
	OtTron tron = std::make_shared<OtTronClass>();
	tron->setType(getMeta());
	return tron;
}
