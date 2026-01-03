//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtUi.h"

#include "OtWidgetStack.h"


//
//	OtWidgetStackClass::render
//

void OtWidgetStackClass::render() {
	// render the first enabled child
	ImGui::PushID(this);

	bool done = false;

	for (size_t i = 0; i < children.size() && !done; i++) {
		if (children[i]->isEnabled()) {
			renderChild(children[i]);
			done = true;
		}
	}

	ImGui::PopID();
}


//
//	OtWidgetStackClass::getMeta
//

OtType OtWidgetStackClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtWidgetStackClass>("WidgetStack", OtWidgetClass::getMeta());
	}

	return type;
}
