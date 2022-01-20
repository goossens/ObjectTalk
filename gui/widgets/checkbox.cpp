//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"
#include "ot/vm.h"

#include "checkbox.h"


//
//	OtCheckboxClass::init
//

void OtCheckboxClass::init(const std::string& l, bool chk, OtObject cb) {
	// save label and callback
	label = l;
	checked = chk;
	callback = cb;
}


//
//	OtCheckboxClass::render
//

void OtCheckboxClass::render() {
	if (ImGui::Checkbox(label.c_str(), &checked)) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(checked));
	}
}


//
//	OtCheckboxClass::getMeta
//

OtType OtCheckboxClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCheckboxClass>("Checkbox", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtCheckboxClass::init));
	}

	return type;
}


//
//	OtCheckboxClass::create
//

OtCheckbox OtCheckboxClass::create() {
	OtCheckbox checkbox = std::make_shared<OtCheckboxClass>();
	checkbox->setType(getMeta());
	return checkbox;
}
