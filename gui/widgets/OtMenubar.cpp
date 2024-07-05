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

#include "OtMenuBar.h"


//
//	OtMenuBarClass::validateChild
//

void OtMenuBarClass::validateChild(OtWidget child) {
	if (!child->isKindOf("Menu")) {
		OtLogFatal("A [MenuBar] can only have [Menu]s as children, not [{}]", child->getType()->getName());
	}
}


//
//	OtMenuBarClass::render
//

void OtMenuBarClass::render() {
	if (ImGui::BeginMainMenuBar()) {
		OtWidgetClass::render();
		ImGui::EndMainMenuBar();
	}
}


//
//	OtMenuBarClass::getMeta
//

OtType OtMenuBarClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtMenuBarClass>("MenuBar", OtWidgetClass::getMeta());
	}

	return type;
}
