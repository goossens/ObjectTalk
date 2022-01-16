//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "application.h"
#include "screen.h"
#include "panel.h"


//
//	OtPanelClass::validateChild
//

void OtPanelClass::validateChild(OtComponent child) {
	if (!child->isKindOf("Widget")) {
		OtExcept("A [Panel] can only have [Widget]s as children, not [%s]", child->getType()->getName().c_str());
	}
}


//
//	OtPanelClass::render
//

void OtPanelClass::render() {
	size_t offset = 0;

	// take menubar into account if there is one
	if (parent) {
		offset = parent->cast<OtScreenClass>()->getMenubarHeight();
	}

	// frame height
	OtApplication application = OtApplicationClass::instance();
	float fw = application->getWidth();
	float fh = application->getHeight() - offset;

	// determine panel dimensions
	float vx = x < 0 ? fw + x * fw / 100.0 : x * fw / 100.0;
	float vy = offset + (y < 0 ? fh + y * fh / 100.0 : y * fh / 100.0);
	float vw = w * fw / 100.0;
	float vh = h * fh / 100.0;

	// position panel
	ImGui::SetNextWindowPos(ImVec2(vx, vy), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(vw, vh), ImGuiCond_Always);

	// determine flags
	int flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("panel", nullptr, flags)) {
		// render all children
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
		OtWidgetClass::render();
	}

	ImGui::End();
}


//
//	OtPanelClass::getMeta
//

OtType OtPanelClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPanelClass>("Panel", OtScreenObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPanelClass::init));
	}

	return type;
}


//
//	OtPanelClass::create
//

OtPanel OtPanelClass::create() {
	OtPanel panel = std::make_shared<OtPanelClass>();
	panel->setType(getMeta());
	return panel;
}
