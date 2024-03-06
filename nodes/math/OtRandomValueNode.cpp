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

#include "OtNumbers.h"

#include "OtNodesFactory.h"


//
//	OtRandomValueNode
//

class OtRandomValueNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Value", value);
	}

	// render custom fields
	void customRendering(float width) override {
		auto old = serialize().dump();
		bool changed = false;

		ImGui::SetNextItemWidth(width - labelWidth);
		changed |= ImGui::InputFloat("Min", &minValue);
		ImGui::SetNextItemWidth(width - labelWidth);
		changed |= ImGui::InputFloat("Max", &maxValue);

		if (changed) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return customWidth;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() * 2.0f;
	}

	// generate random number
	void onExecute() override {
		value = OtRandom(minValue, maxValue);
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["minValue"] = minValue;
		(*data)["maxValue"] = maxValue;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		minValue = data->value("minValue", 0.0f);
		maxValue = data->value("maxValue", 1.0f);
	}

	static constexpr const char* nodeName = "Random";
	static constexpr int nodeCategory = OtNodeClass::math;
	static constexpr int nodeKind = OtNodeClass::varying;
	static constexpr float fieldWidth = 50.0f;
	static constexpr float labelWidth = 30.0f;
	static constexpr float customWidth = fieldWidth + labelWidth;

protected:
	float value = 0.0f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
};

static OtNodesFactoryRegister<OtRandomValueNode> type;
