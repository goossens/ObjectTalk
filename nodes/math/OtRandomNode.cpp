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

	// process the varying context (called for each iteration)
	void processVaryingContext(OtNodeVaryingContext& context) override {
		id = context.index;
	}

	// generate random number
	void onExecute() override {
		if (seed == 0) {
			seed = 1;
		}

		if (outputPin->isDestinationConnected() && outputPin->destinationPin->isVarying()) {
			value = OtHash::toFloat(static_cast<uint32_t>(id), seed) * (maxValue - minValue) + minValue;

		} else {
			value = OtRandom(minValue, maxValue);
		}
	}

	static constexpr const char* nodeName = "Random";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::math;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::varying;

protected:
	size_t id = 0;
	int seed = 1;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	float value = 0.0f;
	OtNodesPin outputPin;
};

static OtNodesFactoryRegister<OtRandomNode> registration;
