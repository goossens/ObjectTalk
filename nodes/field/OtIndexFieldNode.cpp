//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtIndexFieldNode
//

class OtIndexFieldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Index", index);
	}

	// process the varying context (called for each iteration)
	inline void processVaryingContext(OtNodeVaryingContext& context) override {
		index = static_cast<int>(context.index);
	}

	static constexpr const char* nodeName = "Index Field";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::field;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::varying;

protected:
	int index = 0;
};

static OtNodesFactoryRegister<OtIndexFieldNode> registration;
