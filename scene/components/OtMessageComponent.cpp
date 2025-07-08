//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtMessageComponent.h"


//
//	OtMessageComponent::renderUI
//

bool OtMessageComponent::renderUI() {
	bool changed = OtUi::inputMultilineText("Message", &message, ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 3.0f));
	changed |= ImGui::DragFloat("Visible (seconds)", &visibleTime, 0.5f, 0.0f, 60.0f);
	changed |= ImGui::DragFloat("Fade (seconds)", &fadeTime, 0.1f, 0.0f, 2.0f);
	return changed;
}


//
//	OtMessageComponent::serialize
//

nlohmann::json OtMessageComponent::serialize(std::string* /* basedir */) {
	auto data = nlohmann::json::object();
	data["message"] = message;
	data["visibleTime"] = visibleTime;
	data["fadeTime"] = fadeTime;
	return data;
}


//
//	OtMessageComponent::deserialize
//

void OtMessageComponent::deserialize(nlohmann::json data, std::string* /* basedir */) {
	message = data.value("message", "");
	visibleTime = data.value("visibleTime", 0.0f);
	fadeTime = data.value("fadeTime", 0.0f);
}
