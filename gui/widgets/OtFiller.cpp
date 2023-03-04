//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtFiller.h"


//
//	OtFillerClass::init
//

void OtFillerClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			setSize(parameters[0]->operator float());

		case 0:
			break;

		default:
			OtExcept("[Filler] constructor expects up to 1 argument (not %ld)", count);
	}
}


//
//	OtFillerClass::setSize
//

OtObject OtFillerClass::setSize(float s) {
	size = s;
	return OtObject(this);
}


//
//	OtFillerClass::render
//

void OtFillerClass::render() {
	ImGui::Dummy(ImVec2(0, size));
}


//
//	OtFillerClass::getMeta
//

OtType OtFillerClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtFillerClass>("Filler", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtFillerClass::init));
		type->set("setSize", OtFunction::create(&OtFillerClass::setSize));
	}

	return type;
}
