//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtException.h"
#include "OtFunction.h"

#include "OtApp.h"
#include "OtFramework.h"
#include "OtPanel.h"


//
//	OtPanelClass::init
//

void OtPanelClass::init(size_t count, OtObject* parameters) {
	if (count == 4) {
		setScreenArea(
			parameters[0]->operator int(),
			parameters[1]->operator int(),
			parameters[2]->operator int(),
			parameters[3]->operator int()
		);

	} else if (count != 0) {
		OtExcept("[Panel] constructor expects 0 or 4 arguments (not %ld)", count);
	}
}


//
//	OtPanelClass::setScreenArea
//

OtObject OtPanelClass::setScreenArea(int _x, int _y, int _w, int _h) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	return OtObject(this);
}


//
//	OtPanelClass::validateChild
//

void OtPanelClass::validateChild(OtNode child) {
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
	if (hasParent()) {
		offset = OtApp(getParent())->getMenubarHeight();
	}

	// frame height
	OtFramework framework = OtFrameworkClass::instance();
	float fw = framework->getWidth();
	float fh = framework->getHeight() - offset;

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
		OtAppObjectClass::render();
	}

	ImGui::End();
}


//
//	OtPanelClass::getMeta
//

OtType OtPanelClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPanelClass>("Panel", OtAppObjectClass::getMeta());
		type->set("__init__", OtFunction::create(&OtPanelClass::init));
		type->set("setScreenArea", OtFunction::create(&OtPanelClass::setScreenArea));
	}

	return type;
}
