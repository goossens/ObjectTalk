//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtNodesFactory.h"


//
//	OtFloatProbeNode
//

class OtFloatProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value)->addCustomRenderer([&](float width) {
			ImGui::SetNextItemWidth(width);
			ImGui::InputFloat("##value", &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}, fieldWidth);
	}

	static constexpr const char* nodeName = "Float Probe";
	static constexpr int nodeCategory = OtNodeClass::probe;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 120.0f;

protected:
	float value = 0.0f;
};


static OtNodesFactoryRegister<OtFloatProbeNode> type;
