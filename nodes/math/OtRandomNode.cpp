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

#include "OtNumbers.h"

#include "OtNodesFactory.h"


//
//	OtRandomNode
//

class OtRandomNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Min", minValue);
		addInputPin("Max", maxValue);
		addOutputPin("Value", value);
	}

	// generate random number
	void onExecute() override {
		value = OtRandom(minValue, maxValue);
	}

	static constexpr const char* nodeName = "Random";
	static constexpr int nodeCategory = OtNodeClass::math;
	static constexpr int nodeKind = OtNodeClass::varying;

protected:
	float value = 0.0f;
	float minValue = 0.0f;
	float maxValue = 1.0f;
};

static OtNodesFactoryRegister<OtRandomNode> type;
