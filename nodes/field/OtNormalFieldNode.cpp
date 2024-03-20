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

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtNormalFieldNode
//

class OtNormalFieldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("X", x);
		addOutputPin("Y", y);
		addOutputPin("Z", z);
	}

	// process the varying context (called for each iteration)
	void processVaryingContext(OtNodeVaryingContext& context) override {
		if (context.hasVertex) {
			x = context.vertex.normal.x;
			y = context.vertex.normal.y;
			z = context.vertex.normal.z;
		}
	}

	static constexpr const char* nodeName = "Normal Field";
	static constexpr int nodeCategory = OtNodeClass::field;
	static constexpr int nodeKind = OtNodeClass::varying;

protected:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

static OtNodesFactoryRegister<OtNormalFieldNode> type;
