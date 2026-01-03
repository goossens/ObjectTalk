//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
//	OtVector4ProbeNode
//

class OtVector4ProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value)->addCustomRenderer([&](float width) {
			ImGui::SetNextItemWidth(width);
			OtUi::viewVec4("##value", value);
		}, 260.0f);
	}

	static constexpr const char* nodeName = "Vector4 Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	glm::vec4 value{0.0f};
};


static OtNodesFactoryRegister<OtVector4ProbeNode> registration;
