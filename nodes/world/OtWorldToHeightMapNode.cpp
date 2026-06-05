//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "nlohmann/json.hpp"

#include "OtHeightMap.h"
#include "OtThreadPool.h"
#include "OtUi.h"

#include "OtWorld.h"

#include "OtNodesFactory.h"


//
//	OtWorldToHeightMapNode
//

class OtWorldToHeightMapNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("World", world);
		addInputPin("Size", size);
		addOutputPin("Height Map", heightMap);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		if (generating) {
			auto pos = ImGui::GetCursorScreenPos();
			OtUi::spinner(ImVec2(pos.x + itemWidth * 0.5f, pos.y), OtUi::size(1.0f));
		}
	}

	// update node status
	inline bool onUpdate() override {
		// limit values
		size = std::clamp(size, 32, 4096);

		if (generated) {
			std::swap(heightMap, newHeightMap);
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

	// execute asynchronous image generation
	inline void onExecute() override {
		if (world.isValid()) {
			if (generating) {
				moreRequests = true;

			} else {
				generating = true;
				scheduleGeneration();
			}

		} else {
			heightMap.clear();
		}
	}

	static constexpr const char* nodeName = "World to Height Map";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::world;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int size = 32;

	// world component
	OtWorld world;
	OtHeightMap heightMap;

	// work variables
	bool generating = false;
	bool generated = false;
	bool moreRequests = false;
	OtHeightMap newHeightMap;

	// local functions
	void scheduleGeneration() {
		OtThreadPool::run([captureWorld = world, this]() {
			captureWorld.generateHeightMap(newHeightMap, size);
			generated = true;
		});
	}
};


static OtNodesFactoryRegister<OtWorldToHeightMapNode> registration;
