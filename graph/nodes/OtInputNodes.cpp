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
#include "OtInputNodes.h"


//
//	OtGraphNodeFloatInput
//

class OtGraphNodeFloatInput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeFloatInput() : OtGraphNodeClass(name) {}

	// configure node
	inline void configure() override {
		auto pin = addOutputPin("Number", number);

		pin->addRenderer([this, pin] () {
			ImGui::SetNextItemWidth(fieldWidth);

			if (ImGui::InputFloat("##number", &number)) {
				pin->updated = true;
			}
		}, fieldWidth);
	}

	static constexpr const char* name = "Input Float";

protected:
	static constexpr float fieldWidth = 100.0f;
	float number = 0.0f;
};


//
//	OtInputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Input", CLASS::name)

void OtInputNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeFloatInput);
}
