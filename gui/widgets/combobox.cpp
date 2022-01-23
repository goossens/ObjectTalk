//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/array.h"
#include "ot/callback.h"
#include "ot/function.h"
#include "ot/vm.h"

#include "combobox.h"


//
//	OtComboboxClass::init
//

void OtComboboxClass::init(const std::string& t, OtObject vals, OtObject sel, OtObject cb) {
	// save title
	title = t;

	// sanity check
	vals->expectKindOf("Array");

	// get list of values
	for (auto& member : vals->cast<OtArrayClass>()->raw()) {
		values.push_back(member->operator std::string());
	}

	// save current selection and callback;
	selected = sel->operator std::string();

	OtCallbackValidate(cb, 1);
	callback = cb;
}


//
//	OtComboboxClass::render
//

void OtComboboxClass::render() {
	// start combobox
	if (ImGui::BeginCombo(title.c_str(), selected.c_str())) {
		// add our items
		for (auto& value : values) {
			bool isSelectedOne = value == selected;

			if (ImGui::Selectable(value.c_str(), isSelectedOne)) {
				if (selected != value) {
					selected = value;
					OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(selected));
				}
			}

			// ensure selected entry is in focus
			if (isSelectedOne) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}
}


//
//	OtComboboxClass::getMeta
//

OtType OtComboboxClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtComboboxClass>("Combobox", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtComboboxClass::init));
	}

	return type;
}


//
//	OtComboboxClass::create
//

OtCombobox OtComboboxClass::create() {
	OtCombobox combobox = std::make_shared<OtComboboxClass>();
	combobox->setType(getMeta());
	return combobox;
}
