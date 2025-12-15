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
//	OtCombineXyzwNode
//

class OtCombineXyzwNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("X", x);
		addInputPin("Y", y);
		addInputPin("Z", z);
		addInputPin("W", w);
		addOutputPin("Value", value);
	}

	// combine values
	inline void onExecute() override {
		value = glm::vec4(x, y, z, w);
	}

	static constexpr const char* nodeName = "Combine XYZW";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

private:
	// properties
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
	float w{0.0f};
	glm::vec4 value{0.0f};
};

static OtNodesFactoryRegister<OtCombineXyzwNode> registration;
