//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtFaust.h"


//
//	OtFaust::render
//

bool OtFaust::render() {
	bool first = true;
	bool changed = false;

	for (auto control : ui.controls) {
		if (first) {
			first = false;

		} else {
			ImGui::SameLine();
		}

		ImGui::PushID(control.get());

		changed |=OtUi::knob(
			control->getLabel(),
			control->getVariable(),
			control->getMin(),
			control->getMax(),
			control->getFormat());

		ImGui::PopID();
	}

	return changed;
}


//
//	OtFaust::getWidth
//

float OtFaust::getWidth() {
	return OtUi::knobWidth(ui.controls.size());
}


//
//	OtFaust::getHeight
//

float OtFaust::getHeight() {
	return OtUi::knobHeight();
}


//
//	OtFaust::serialize
//

void OtFaust::serialize(nlohmann::json* data) {
	for (auto control : ui.controls) {
		(*data)[control->getName()] = control->get();
	}
}


//
//	OtFaust::deserialize
//

void OtFaust::deserialize(nlohmann::json* data) {
	for (auto control : ui.controls) {
		control->set(data->value(control->getName(), control->getDefault()));
	}
}
