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

#include "OtHash.h"
#include "OtNumbers.h"

#include "OtNodesFactory.h"


//
//	OtRandomNode
//

class OtRandomNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Seed", seed);
		addInputPin("Min", minValue);
		addInputPin("Max", maxValue);
		outputPin = addOutputPin("Value", value);
	}

	// process the varying context (called each iteration)
	inline void processVaryingContext(OtNodeVaryingContext& context) override {
		varying = true;
		id = context.index;
	}

	// generate random number
	inline void onExecute() override {
		if (seed == 0) {
			seed = 1;
		}

		if (varying) {
			value = OtHash::toFloat(static_cast<uint32_t>(id), seed) * (maxValue - minValue) + minValue;
			varying = false;

		} else {
			value = OtRandom(minValue, maxValue);
		}
	}

	static constexpr const char* nodeName = "Random Float";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::varying;

private:
	// properties
	bool varying = false;
	size_t id = 0;
	int seed = 1;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = 0.0f;
	OtNodesPin outputPin;
};

static OtNodesFactoryRegister<OtRandomNode> registration;
