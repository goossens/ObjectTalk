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
//	OtPositionFieldNode
//

class OtPositionFieldNode : public OtNodeClass {
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
			x = context.vertex.position.x;
			y = context.vertex.position.y;
			z = context.vertex.position.z;
		}
	}

	static constexpr const char* nodeName = "Position Field";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::field;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::varying;

protected:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};

static OtNodesFactoryRegister<OtPositionFieldNode> type;
