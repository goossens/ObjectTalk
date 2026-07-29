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
#include "OtUi.h"

#include "OtWorld.h"

#include "OtNodesFactory.h"


//
//	OtWorldGeneratorNode
//

class OtWorldGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Region Grid Size", regionGridSize);
		addInputPin("Seed", seed);
		addInputPin("Ruggedness", ruggedness);
		static constexpr const char* outputLabel = "World";

		addOutputPin(outputLabel, world)->addCustomRenderer([this](float width) {
			if (generating) {
				ImGui::SetNextItemWidth(width);
				ImGui::ProgressBar(static_cast<float>(-ImGui::GetTime()), ImVec2(), "Generating...");

			} else {
				OtUi::hSpacer(width - ImGui::CalcTextSize(outputLabel).x);
				OtUi::text(outputLabel);
			}

		}, OtUi::size(8.0f));
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
	int regionGridSize = 32;
	int seed = 1;
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
		newWorld.setRegionGridSize(std::clamp(regionGridSize, 4, 4096));
		newWorld.setSeed(std::clamp(seed, 1, 1024));
		newWorld.setRuggedness(std::clamp(ruggedness, 0.0f, 1.0f));

		OtThreadPool::run([this]() {
			newWorld.generate();
			generated = true;
		});
	}
};


static OtNodesFactoryRegister<OtWorldGeneratorNode> registration;
