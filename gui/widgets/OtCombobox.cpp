//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtArray.h"
#include "OtFunction.h"
#include "OtUi.h"
#include "OtVM.h"

#include "OtCombobox.h"


//
//	OtComboboxClass::init
//

void OtComboboxClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 4:
			setCallback(parameters[3]);

		case 3:
			setOptions(parameters[2]);

		case 2:
			setValue(parameters[1]->operator std::string());

		case 1:
			setLabel(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[Combobox] constructor expects up to 4 arguments (not {})", count);
	}
}


//
//	OtComboboxClass::setOptions
//

OtObject OtComboboxClass::setOptions(OtObject vals) {
	// sanity check
	vals->expectKindOf("Array");

	// get list of values
	options.clear();

	for (auto& member : OtArray(vals)->raw()) {
		options.push_back(member->operator std::string());
	}

	return OtWidget(this);
}


//
//	OtComboboxClass::render
//

void OtComboboxClass::render() {
	ImGui::PushID(this);

	// start combobox
	if (ImGui::BeginCombo(label.c_str(), value.c_str())) {
		// add our items
		for (auto& option : options) {
			ImGui::PushID(&option);
			bool isSelectedOne = option == value;

			if (ImGui::Selectable(option.c_str(), isSelectedOne)) {
				if (value != option) {
					value = option;
					OtVM::instance()->callMemberFunction(callback, "__call__", OtValue<std::string>::encode(value));
				}
			}

			// ensure selected entry is in focus
			if (isSelectedOne) {
				ImGui::SetItemDefaultFocus();
			}

			ImGui::PopID();
		}

		ImGui::EndCombo();
	}

	ImGui::PopID();
}


//
//	OtComboboxClass::getMeta
//

OtType OtComboboxClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtComboboxClass>("Combobox", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtComboboxClass::init));
		type->set("setLabel", OtFunction::create(&OtComboboxClass::setLabel));
		type->set("setValue", OtFunction::create(&OtComboboxClass::setValue));
		type->set("setOptions", OtFunction::create(&OtComboboxClass::setOptions));
		type->set("getValue", OtFunction::create(&OtComboboxClass::getValue));
	}

	return type;
}
