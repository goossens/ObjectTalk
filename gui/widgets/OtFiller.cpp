//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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

OtObject OtFillerClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			setSize(parameters[0]->operator float());

		case 0:
			break;

		default:
			OtExcept("[Filler] constructor expects up to 1 argument (not %ld)", count);
	}

	return nullptr;
}


//
//	OtFillerClass::setSize
//

OtObject OtFillerClass::setSize(float s) {
	size = s;
	return shared();
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
		type = OtTypeClass::create<OtFillerClass>("Filler", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtFillerClass::init));
		type->set("setSize", OtFunctionClass::create(&OtFillerClass::setSize));
	}

	return type;
}


//
//	OtFillerClass::create
//

OtFiller OtFillerClass::create() {
	OtFiller filler = std::make_shared<OtFillerClass>();
	filler->setType(getMeta());
	return filler;
}
