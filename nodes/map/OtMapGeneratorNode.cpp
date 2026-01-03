//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtMap.h"

#include "OtNodesFactory.h"


//
//	OtMapGeneratorNode
//

class OtMapGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Size", size);
		addInputPin("Seed", seed);
		addInputPin("Ruggedness", ruggedness);
		addOutputPin("Map", map);
	}

	// validate input parameters
	inline void onValidate() override {
		seed = std::clamp(seed, 1, 256);
		size = std::clamp(size, 4, 128);
		ruggedness = std::clamp(ruggedness, 0.0f, 1.0f);
	}

	// run the map generator
	inline void onExecute() override {
		map.update(seed, size, ruggedness);
	}

	static constexpr const char* nodeName = "Map Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::map;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int seed = 1;
	int size = 32;
	float ruggedness = 0.5f;

	// map component
	OtMap map;
};


static OtNodesFactoryRegister<OtMapGeneratorNode> registration;
