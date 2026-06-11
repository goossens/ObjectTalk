//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtHeightMap.h"
#include "OtThreadPool.h"
#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtErodeHeightMapNode
//

class OtErodeHeightMapNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("World", heightMap);
		addOutputPin("Height Map", erodedHeightMap);
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
		if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			future.get();
			auto version = erodedHeightMap.getVersion();
			erodedHeightMap = workHeightMap;
			erodedHeightMap.setVersion(version + 1);

			if (currentRun < runs) {
				currentRun++;
				scheduleGeneration();

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
		if (heightMap.isValid()) {
			if (!generating) {
				generating = true;
				workHeightMap = heightMap.clone();
				scheduleGeneration();
			}

		} else {
			erodedHeightMap.clear();
		}
	}

	static constexpr const char* nodeName = "Erode Height Map";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::world;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int runs = 100;
	int currentRun = 0;

	// world component
	OtHeightMap heightMap;
	OtHeightMap workHeightMap;
	OtHeightMap erodedHeightMap;

	// work variables
	std::future<void> future;
	bool generating = false;

	// local functions
	void scheduleGeneration() {
		future = OtThreadPool::submit<void>([whm = workHeightMap, cr = currentRun]() mutable {
			whm.erode(cr, 1000);
		});
	}
};


static OtNodesFactoryRegister<OtErodeHeightMapNode> registration;
