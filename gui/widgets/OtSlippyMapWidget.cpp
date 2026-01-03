//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtSlippyMapWidget.h"


//
//	OtSlippyMapWidgetClass::init
//

void OtSlippyMapWidgetClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			resolution = parameters[3]->operator double();
			[[fallthrough]];

		case 1:
			center.lat = parameters[1]->operator double();
			center.lon = parameters[2]->operator double();
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogError("[MapView] constructor expects 0, 2 or 3 arguments (not {})", count);
	}
}


//
//	OtSlippyMapWidgetClass::render
//

void OtSlippyMapWidgetClass::render() {
	ImGui::PushID(this);
	map.render("map", center, resolution);
	ImGui::PopID();
}


//
//	OtSlippyMapWidgetClass::getMeta
//

OtType OtSlippyMapWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtSlippyMapWidgetClass>("SlippyMap", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtSlippyMapWidgetClass::init));
		type->set("addOsmLayer", OtFunction::create(&OtSlippyMapWidgetClass::addOsmLayer));
	}

	return type;
}
