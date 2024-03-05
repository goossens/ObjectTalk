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
//	OtCombineXyzNode
//

class OtCombineXyzNode : public OtNodeClass {
public:
	// constructor
	inline OtCombineXyzNode() : OtNodeClass(name, category) {}

	// configure node
	inline void configure() override {
		addInputPin("X", x);
		addInputPin("Y", y);
		addInputPin("Z", z);
		addOutputPin("Value", value);
	}

	// combine values
	void onExecute() override {
		value = glm::vec3(x, y, z);
	}

	static constexpr const int category = OtNodeClass::math;
	static constexpr const char* name = "Combine XYZ";

protected:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	glm::vec3 value{0.0f};
};

static OtNodesFactoryRegister<OtCombineXyzNode> type("Math");
