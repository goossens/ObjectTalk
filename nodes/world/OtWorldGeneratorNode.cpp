//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtWorld.h"

#include "OtNodesFactory.h"


//
//	OtWorldGeneratorNode
//

class OtWorldGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Size", size);
		addInputPin("Seed", seed);
		addInputPin("Ruggedness", ruggedness);
		addOutputPin("World", world);
	}

	// validate input parameters
	inline void onValidate() override {
		seed = std::clamp(seed, 1, 256);
		size = std::clamp(size, 4, 128);
		ruggedness = std::clamp(ruggedness, 0.0f, 1.0f);
	}

	// run the world generator
	inline void onExecute() override {
		world.setSeed(seed);
		world.setSize(size);
		world.setRuggedness(ruggedness);
		world.generate();
	}

	static constexpr const char* nodeName = "World Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::world;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int seed = 1;
	int size = 32;
	float ruggedness = 0.5f;

	// world component
	OtWorld world;
};


static OtNodesFactoryRegister<OtWorldGeneratorNode> registration;
