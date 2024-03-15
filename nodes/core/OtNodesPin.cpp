//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNode.h"
#include "OtNodesPin.h"


//
//	OtNodesPinClass::serialize
//

nlohmann::json OtNodesPinClass::serialize(std::string* basedir) {
	auto data = nlohmann::json::object();
	data["type"] = getTypeName();
	data["id"] = id;
	data["name"] = name;

	if (isInput() && !isSourceConnected() && inputConfig) {
		inputConfig->serializeValue(&data);
	}

	return data;
}


//
//	OtNodesPinClass::deserialize
//

void OtNodesPinClass::deserialize(nlohmann::json data, bool restoreIDs, std::string* basedir) {
	// restore ID (if required)
	if (restoreIDs) {
		id = data["id"];
	}

	if (isInput() && !isSourceConnected() && inputConfig) {
		inputConfig->deserializeValue(&data);
	}
}


//
//	OtNodesPinCreateInputConfig
//

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(bool& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 30.0f; },

		[&](OtNodesPin pin, float width) {
			auto node = pin->node;
			auto old = node->serialize().dump();
			ImGui::PushID(&value);
			ImGui::SetNextItemWidth(width);

			if (OtUiToggleButton("##value", &value)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(int& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 60.0f; },

		[&](OtNodesPin pin, float width) {
			auto node = pin->node;
			auto old = node->serialize().dump();
			ImGui::PushID(&value);
			ImGui::SetNextItemWidth(width);

			auto absValue = std::abs(value);
			int speed;

			if (absValue < 100) {
				speed = 1;

			} else if (absValue < 1000) {
				speed = 10;

			} else {
				speed = 100;
			}

			if (ImGui::DragInt("##value", &value, speed)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(float& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 60.0f; },

		[&](OtNodesPin pin, float width) {
			auto node = pin->node;
			auto old = node->serialize().dump();
			ImGui::PushID(&value);
			ImGui::SetNextItemWidth(width);

			auto absValue = std::abs(value);
			float speed;
			const char* format;

			if (absValue < 1.0f) {
				speed = 0.01f;
				format = "%.3f";

			} else if (absValue < 10.0f) {
				speed = 0.1f;
				format = "%.2f";

			} else if (absValue < 100.0f) {
				speed = 1.0f;
				format = "%.1f";

			} else {
				speed = 10.0f;
				format = "%.0f";
			}

			if (ImGui::DragFloat("##value", &value, speed, 0.0f, 0.0f, format)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(std::string& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 60.0f; },

		[&](OtNodesPin pin, float width) {
			auto node = pin->node;
			auto old = node->serialize().dump();
			ImGui::PushID(&value);
			ImGui::SetNextItemWidth(width);

			OtUiInputText("##value", value, ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue);
			if (ImGui::IsItemDeactivated()) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec3& value) {
	return nullptr;
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec4& value) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtImage& value) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtTexture& value) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtGeometry& value) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtInstances& value) { return nullptr; }
