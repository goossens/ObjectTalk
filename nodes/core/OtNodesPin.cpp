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

#include "OtGlm.h"
#include "OtUi.h"

#include "OtNode.h"
#include "OtNodesPin.h"


//
//	OtNodesPinClass::serialize
//

nlohmann::json OtNodesPinClass::serialize(std::string* /* basedir */) {
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

void OtNodesPinClass::deserialize(nlohmann::json data, bool restoreIDs, std::string* /* basedir */) {
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
		[]() { return ImGui::GetFrameHeight() * 1.55f; },

		[&](OtNodesPin pin, float width) {
			auto node = pin->node;
			auto old = node->serialize().dump();
			ImGui::PushID(&value);
			OtUi::hSpacer(width - ImGui::GetFrameHeight() * 1.55f);

			if (OtUi::toggleButton("##value", &value)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
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

			if (OtUi::dragInt("##value", &value)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
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

			if (OtUi::dragFloat("##value", &value)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(std::string& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 100.0f; },

		[&](OtNodesPin pin, float width) {
			auto text = value;
			ImGui::PushID(&value);
			ImGui::SetNextItemWidth(width);

			if (OtUi::inputText("##value", &text)) {
				auto node = pin->node;
				node->oldState = node->serialize().dump();
				value = text;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtColor& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 20.0f; },

		[&](OtNodesPin pin, float width) {
			auto node = pin->node;
			auto old = node->serialize().dump();
			ImGui::PushID(&value);
			ImGui::SameLine(0.0f, width - 10.0f);

			ImGuiColorEditFlags flags =
				ImGuiColorEditFlags_Float |
				ImGuiColorEditFlags_NoInputs |
				ImGuiColorEditFlags_AlphaBar;

			if (ImGui::ColorEdit4("##value", value.data(), flags)) {
				node->oldState = old;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec2& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 120.0f; },

		[&](OtNodesPin pin, float width) {
			auto vec = value;
			ImGui::PushID(&value);
			OtUi::hSpacer(width - 120.0f);
			ImGui::SetNextItemWidth(120.0f);

			if (OtUi::editVec2("##value", &vec)) {
				auto node = pin->node;
				node->oldState = node->serialize().dump();
				value = vec;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec3& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 180.0f; },

		[&](OtNodesPin pin, float width) {
			auto vec = value;
			ImGui::PushID(&value);
			OtUi::hSpacer(width - 180.0f);
			ImGui::SetNextItemWidth(180.0f);

			if (OtUi::editVec3("##value", &vec)) {
				auto node = pin->node;
				node->oldState = node->serialize().dump();
				value = vec;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec4& value) {
	return new OtNodesPinInputConfig{
		[&](nlohmann::json* data) { (*data)["value"] = value; },
		[&](nlohmann::json* data) { if (data->contains("value")) { value = (*data)["value"]; } },
		[]() { return 240.0f; },

		[&](OtNodesPin pin, float width) {
			auto vec = value;
			ImGui::PushID(&value);
			OtUi::hSpacer(width - 240.0f);
			ImGui::SetNextItemWidth(240.0f);

			if (OtUi::editVec4("##value", &vec)) {
				auto node = pin->node;
				node->oldState = node->serialize().dump();
				value = vec;
				node->newState = node->serialize().dump();
				node->needsEvaluating = true;
				node->needsSaving = true;
			}

			ImGui::PopID();
		}
	};
}

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtFont& /* value */) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtImage& /* value */) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtTexture& /* value */) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtShape& /* value */) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtGeometry& /* value */) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtManifold& /* value */) { return nullptr; }
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtInstances& /* value */) { return nullptr; }
