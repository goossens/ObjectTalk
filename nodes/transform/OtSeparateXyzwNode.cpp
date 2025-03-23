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
//	OtSeparateXyzwNode
//

class OtSeparateXyzwNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value);
		addOutputPin("X", x);
		addOutputPin("Y", y);
		addOutputPin("Z", z);
		addOutputPin("W", w);
	}

	// separate values
	inline void onExecute() override {
		x = value.x;
		y = value.y;
		z = value.z;
		w = value.z;
	}

	static constexpr const char* nodeName = "Separate XYZW";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::transform;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::flexible;

protected:
	glm::vec4 value{0.0f};
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
	float w{0.0f};
};

static OtNodesFactoryRegister<OtSeparateXyzwNode> registration;
