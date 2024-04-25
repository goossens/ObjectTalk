//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtNodesFactory.h"


//
//	OtSeparateXyzNode
//

class OtSeparateXyzNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Value", value);
		addOutputPin("X", x);
		addOutputPin("Y", y);
		addOutputPin("Z", z);
	}

	// separate values
	void onExecute() override {
		x = value.x;
		y = value.y;
		z = value.z;
	}

	static constexpr const char* nodeName = "Separate XYZ";
	static constexpr int nodeCategory = OtNodeClass::transform;
	static constexpr int nodeKind = OtNodeClass::flexible;

protected:
	glm::vec3 value{0.0f};
	float x{0.0f};
	float y{0.0f};
	float z{0.0f};
};

static OtNodesFactoryRegister<OtSeparateXyzNode> type;
