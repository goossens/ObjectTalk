//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtStringInputNode
//

class OtStringInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addCustomRenderer([this](float width) {
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				oldState = old;
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
		OtUiInputText("##value", &value, ImGuiInputTextFlags_NoUndoRedo | ImGuiInputTextFlags_EnterReturnsTrue);
		return ImGui::IsItemDeactivated();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		value = data->value("value", "");
	}

	static constexpr const char* nodeName = "String Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 180.0f;

protected:
	std::string value = "";
};

static OtNodesFactoryRegister<OtStringInputNode> type;
