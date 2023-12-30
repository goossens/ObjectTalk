//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtGraphNode.h"
#include "OtOutputNodes.h"


//
//	OtGraphNodeFloatOutput
//

class OtGraphNodeFloatOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeFloatOutput() : OtGraphNodeClass(name) {}

	// configure node
	inline void configure() override {
		auto pin = addInputPin("Number", number);

		pin->addRenderer([&] () {
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::InputFloat("##number", &number, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}, fieldWidth);
	}

	static constexpr const char* name = "Output Float";

protected:
	static constexpr float fieldWidth = 100.0f;
	float number = 0.0f;
};


//
//	OtOutputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Output", CLASS::name)

void OtOutputNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeFloatOutput);
}
