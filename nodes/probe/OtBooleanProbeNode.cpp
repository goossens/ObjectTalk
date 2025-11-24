//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtBooleanProbeNode
//

class OtBooleanProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value)->addCustomRenderer([&](float width) {
			ImGui::SetNextItemWidth(width);
			OtUi::readonlyBool("##value", value);
		}, 100.0f);
	}

	static constexpr const char* nodeName = "Boolean Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// proterties
	bool value = false;
};


static OtNodesFactoryRegister<OtBooleanProbeNode> registration;
