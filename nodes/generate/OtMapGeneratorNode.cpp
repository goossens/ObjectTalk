//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <set>
#include <vector>

#include "delaunator.h"
#include "glm/glm.hpp"

#include "OtHash.h"
#include "OtLog.h"

#include "OtMap.h"
#include "OtNoise.h"

#include "OtNodesFactory.h"
#include "OtTextureGeneratorNode.h"


//
//	OtMapGeneratorNode
//

class OtMapGeneratorNode : public OtTextureGeneratorNode {
public:
	// configure node
	inline void configure() override {
		OtTextureGeneratorNode::configure();
		addInputPin("Size", size);
		addInputPin("Seed", seed);
		addInputPin("Ruggedness", ruggedness);
	}

	// validate input parameters
	inline void onValidate() override {
		seed = std::clamp(seed, 1, 256);
		size = std::clamp(size, 4, 128);
		ruggedness = std::clamp(ruggedness, 0.0f, 1.0f);
	}

	// run the map generator
	inline void onGenerate(OtFrameBuffer& output) override {
		map.update(seed, size, ruggedness);
		map.render(output);
	}

	static constexpr const char* nodeName = "Map Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::generate;
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
