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

#include "OtNodesFactory.h"


//
//	OtIntegerInputNode
//

class OtIntegerInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addCustomRenderer([this](float width) {
			ImGui::SetNextItemWidth(width);
			ImGui::InputInt("##value", &value, 1, 100, ImGuiInputTextFlags_NoUndoRedo);

			if (ImGui::IsItemActivated()) {
				oldState = serialize().dump();
			}

			if (ImGui::IsItemDeactivated()) {
				newState = serialize().dump();

				if (newState != oldState) {
					needsEvaluating = true;
					needsSaving = true;
				}
			}
		}, fieldWidth);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return ImGui::InputInt("##value", &value, 1, 100, ImGuiInputTextFlags_NoUndoRedo);
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		value = data->value("value", 0);
	}

	static constexpr const char* nodeName = "Integer Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 120.0f;

protected:
	int value = 0;
};

static OtNodesFactoryRegister<OtIntegerInputNode> type;
