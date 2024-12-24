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
//	OtUvFieldNode
//

class OtUvFieldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("U", u);
		addOutputPin("V", v);
	}

	// process the varying context (called for each iteration)
	void processVaryingContext(OtNodeVaryingContext& context) override {
		if (context.hasVertex) {
			u = context.vertex.uv.x;
			v = context.vertex.uv.y;
		}
	}

	static constexpr const char* nodeName = "UV Field";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::field;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::varying;

protected:
	float u = 0.0f;
	float v = 0.0f;
};

static OtNodesFactoryRegister<OtUvFieldNode> type;
