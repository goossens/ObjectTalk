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
//	OtFloatToVectorNode
//

class OtFloatToVectorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addOutputPin("Value", value);
	}

	// combine values
	void onExecute() override {
		value = glm::vec3(a, a, a);
	}

	static constexpr const char* nodeName = "Float to Vector";
	static constexpr int nodeCategory = OtNodeClass::transformer;
	static constexpr int nodeKind = OtNodeClass::flexible;

protected:
	float a{0.0f};
	glm::vec3 value{0.0f};
};

static OtNodesFactoryRegister<OtFloatToVectorNode> type;
