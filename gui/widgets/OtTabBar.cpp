//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtLog.h"

#include "OtTab.h"
#include "OtTabBar.h"


//
//	OtTabBarClass::validateChild
//

void OtTabBarClass::validateChild(OtWidget child) {
	if (!child.isKindOf<OtTabClass>()) {
		OtLogError("A [TabBar] can only have [Tab]s as children, not [{}]", child.getTypeName());
	}
}


//
//	OtTabBarClass::render
//

void OtTabBarClass::render() {
	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		renderChildren();
		ImGui::EndTabBar();
	}
}


//
//	OtTabBarClass::getMeta
//

OtType OtTabBarClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTabBarClass>("TabBar", OtWidgetClass::getMeta());
	}

	return type;
}
