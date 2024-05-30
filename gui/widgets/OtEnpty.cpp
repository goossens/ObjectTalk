//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtEmpty.h"


//
//	OtEmptyClass::render
//

void OtEmptyClass::render() {
	ImGui::PushID(this);
	ImGui::PopID();
}


//
//	OtEmptyClass::getMeta
//

OtType OtEmptyClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtEmptyClass>("Empty", OtWidgetClass::getMeta());
//		type->set("setSplit", OtFunction::create(&OtEmptyClass::setSplit));
	}

	return type;
}
