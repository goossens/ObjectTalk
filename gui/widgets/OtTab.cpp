//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtUi.h"

#include "OtTab.h"


//
//	OtTabClass::init
//

void OtTabClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			setLabel(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogFatal("[Tab] constructor expects up to 1 argument (not {})", count);
	}
}


//
//	OtTabClass::render
//

void OtTabClass::render() {
	if (ImGui::BeginTabItem(label.c_str())) {
		renderChildren();
		ImGui::EndTabItem();
	}
}


//
//	OtTabClass::getMeta
//

OtType OtTabClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTabClass>("Tab", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTabClass::init));
		type->set("setLabel", OtFunction::create(&OtTabClass::setLabel));
	}

	return type;
}
