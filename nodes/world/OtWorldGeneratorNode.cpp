//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtThreadPool.h"

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

	// update node status
	inline bool onUpdate() override {
		if (generated) {
			std::swap(world, newWorld);
			generated = false;

			if (moreRequests) {
				scheduleGeneration();
				moreRequests = false;

			} else {
				generating = false;
			}

			return true;

		} else {
			return false;
		}
	}

	// execute asynchronous world generation
	inline void onExecute() override {
		if (generating) {
			moreRequests = true;

		} else {
			generating = true;
			scheduleGeneration();
		}
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

	// work variables
	bool generating = false;
	bool generated = false;
	bool moreRequests = false;
	OtWorld newWorld;

	// local functions
	void scheduleGeneration() {
		newWorld.setSeed(seed);
		newWorld.setSize(size);
		newWorld.setRuggedness(ruggedness);

		OtThreadPool::run([this]() {
			newWorld.generate();
			generated = true;
		});
	}
};


static OtNodesFactoryRegister<OtWorldGeneratorNode> registration;
