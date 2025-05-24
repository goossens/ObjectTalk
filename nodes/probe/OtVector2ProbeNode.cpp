//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtVector2ProbeNode
//

class OtVector2ProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value)->addCustomRenderer([&](float width) {
			ImGui::SetNextItemWidth(width);
			OtUi::viewVec2("##value", value);
		}, 130.0f);
	}

	static constexpr const char* nodeName = "Vector2 Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	glm::vec2 value{0.0f};
};


static OtNodesFactoryRegister<OtVector2ProbeNode> registration;
