//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtNodesFactory.h"


//
//	OtCombineXyNode
//

class OtCombineXyNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("X", x);
		addInputPin("Y", y);
		addOutputPin("Value", value);
	}

	// combine values
	inline void onExecute() override {
		value = glm::vec2(x, y);
	}

	static constexpr const char* nodeName = "Combine XY";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

private:
	// proterties
	float x{0.0f};
	float y{0.0f};
	glm::vec2 value{0.0f};
};

static OtNodesFactoryRegister<OtCombineXyNode> registration;
