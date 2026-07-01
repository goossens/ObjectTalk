//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <chrono>
#include <cmath>
#include <format>
#include <future>

#include "nlohmann/json.hpp"

#include "OtThreadPool.h"
#include "OtUi.h"

#include "OtHeightMap.h"

#include "OtNodesFactory.h"


//
//	OtIslandizeHeightMapNode
//

class OtIslandizeHeightMapNode : public OtNodeClass {
public:
	// destructor
	~OtIslandizeHeightMapNode() {
		if (generating && future.valid()) {
			future.wait();
		}
	}

	// configure node
	inline void configure() override {
		addInputPin("Input", heightMap);
		addOutputPin("Output", IslandizedHeightMap);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		return OtUi::selectorEnum("##distance", &distance, OtHeightMap::distanceFunctions, OtHeightMap::distanceFunctionCount);
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::size(10.0f);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["distance"] = distance;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		distance = data->value("distance", OtHeightMap::DistanceFunction::squareBump);
	}

	// update node status
	inline bool onUpdate() override {
		if (generating) {
			if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				future.get();
				IslandizedHeightMap = workHeightMap;

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

		} else {
			return false;
		}
	}

	// execute asynchronous image generation
	inline void onExecute() override {
		if (generating) {
			moreRequests = true;

		} else {
			generating = true;
			scheduleGeneration();
		}
	}

	static constexpr const char* nodeName = "Islandize Height Map";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::heightmap;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtHeightMap::DistanceFunction distance = OtHeightMap::DistanceFunction::squareBump;

	// work component
	OtHeightMap heightMap;
	OtHeightMap workHeightMap;
	OtHeightMap IslandizedHeightMap;

	// work variables
	std::future<void> future;
	bool generating = false;
	bool moreRequests = false;

	// local functions
	void scheduleGeneration() {
		future = OtThreadPool::submit<void>([this]() {
			// ensure output has the right configuration
			workHeightMap = heightMap.clone();

			// run the islandizer
			workHeightMap.islandize(distance);
		});
	}
};


static OtNodesFactoryRegister<OtIslandizeHeightMapNode> registration;
