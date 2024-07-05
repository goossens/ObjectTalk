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

#include "OtTabBar.h"


//
//	OtTabBarClass::validateChild
//

void OtTabBarClass::validateChild(OtWidget child) {
	if (!child->isKindOf("Tab")) {
		OtLogFatal("A [TabBar] can only have [Tab]s as children, not [{}]", child->getType()->getName());
	}
}


//
//	OtTabBarClass::render
//

void OtTabBarClass::render() {
	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_AutoSelectNewTabs)) {
		for (auto& child : children) {
			child->render();
		}

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
