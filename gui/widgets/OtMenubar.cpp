//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtLog.h"

#include "OtMenubar.h"


//
//	OtMenubarClass::validateChild
//

void OtMenubarClass::validateChild(OtWidget child) {
	if (!child->isKindOf("Menu")) {
		OtLogFatal("An [Menubar] can only have [Menu]s as children, not [{}]", child->getType()->getName());
	}
}


//
//	OtMenubarClass::render
//

void OtMenubarClass::render() {
	if (ImGui::BeginMainMenuBar()) {
		OtWidgetClass::render();
		ImGui::EndMainMenuBar();
	}
}


//
//	OtMenubarClass::getMeta
//

OtType OtMenubarClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtMenubarClass>("Menubar", OtWidgetClass::getMeta());
	}

	return type;
}
