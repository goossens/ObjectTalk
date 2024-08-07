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
//	OtIndexFieldNode
//

class OtIndexFieldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Index", index);
	}

	// process the varying context (called for each iteration)
	void processVaryingContext(OtNodeVaryingContext& context) override {
		index = context.index;
	}

	static constexpr const char* nodeName = "Index Field";
	static constexpr int nodeCategory = OtNodeClass::field;
	static constexpr int nodeKind = OtNodeClass::varying;

protected:
	int index = 0;
};

static OtNodesFactoryRegister<OtIndexFieldNode> type;
