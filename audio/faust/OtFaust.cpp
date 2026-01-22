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
	auto first = true;
	auto changed = false;

	for (auto& element : ui.elements) {
		switch (element.getType()) {
			case UiElement::Type::box:
			case UiElement::Type::vertical:
			case UiElement::Type::horizontal:
			case UiElement::Type::close:
				break;

			case UiElement::Type::button:
				if (first) { first = false; } else { ImGui::SameLine(); }

				changed |= OtUi::buttonKnob(
					element.getLabel(),
					element.getVariable(),
					element.getOnLabel(),
					element.getOffLabel());

				break;

			case UiElement::Type::control:
				if (first) { first = false; } else { ImGui::SameLine(); }
				ImGui::PushID(&element);

				changed |= OtUi::knob(
					element.getLabel(),
					element.getVariable(),
					element.getMin(),
					element.getMax(),
					element.getFormat());

				ImGui::PopID();
				break;
		}
	}

	return changed;
}


//
//	OtFaust::getWidth
//

float OtFaust::getWidth() {
	return OtUi::knobWidth(ui.cols);
}


//
//	OtFaust::getHeight
//

float OtFaust::getHeight() {
	return OtUi::knobHeight(ui.rows);
}


//
//	OtFaust::serialize
//

void OtFaust::serialize(nlohmann::json* data) {
	for (auto& element : ui.elements) {
		auto variable = element.getVariable();

		if (variable) {
			(*data)[element.getLabel()] = *variable;
		}
	}
}


//
//	OtFaust::deserialize
//

void OtFaust::deserialize(nlohmann::json* data) {
	for (auto& element : ui.elements) {
		auto variable = element.getVariable();

		if (variable) {
			*variable = data->value(element.getLabel(), element.getDefault());
		}
	}
}
