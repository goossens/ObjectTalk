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

#include "OtNodesFactory.h"


//
//	OtBoolInputNode
//

class OtBoolInputNode : public OtNodeClass {
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
		return OtUiToggleButton("##value", &value);
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		value = data->value("value", false);
	}

	static constexpr const char* nodeName = "Boolean Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 100.0f;

protected:
	bool value = false;
};

static OtNodesFactoryRegister<OtBoolInputNode> type;
