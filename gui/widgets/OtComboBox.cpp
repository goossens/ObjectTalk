//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtArray.h"
#include "OtFunction.h"
#include "OtVM.h"

#include "OtUi.h"

#include "OtComboBox.h"


//
//	OtComboBoxClass::init
//

void OtComboBoxClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 4:
			setCallback(parameters[3]);
			[[fallthrough]];

		case 3:
			setOptions(parameters[2]);
			[[fallthrough]];

		case 2:
			setValue(parameters[1]->operator std::string());
			[[fallthrough]];

		case 1:
			setLabel(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogFatal("[ComboBox] constructor expects up to 4 arguments (not {})", count);
	}
}


//
//	OtComboBoxClass::setOptions
//

OtObject OtComboBoxClass::setOptions(OtObject vals) {
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
//	OtComboBoxClass::render
//

void OtComboBoxClass::render() {
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
					OtVM::callMemberFunction(callback, "__call__", OtValue<std::string>::encode(value));
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
//	OtComboBoxClass::getMeta
//

OtType OtComboBoxClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtComboBoxClass>("ComboBox", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtComboBoxClass::init));
		type->set("setLabel", OtFunction::create(&OtComboBoxClass::setLabel));
		type->set("setValue", OtFunction::create(&OtComboBoxClass::setValue));
		type->set("setOptions", OtFunction::create(&OtComboBoxClass::setOptions));
		type->set("getValue", OtFunction::create(&OtComboBoxClass::getValue));
	}

	return type;
}
