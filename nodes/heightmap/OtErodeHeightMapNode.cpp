//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <chrono>
#include <format>
#include <future>

#include "OtThreadPool.h"
#include "OtUi.h"

#include "OtHeightMap.h"

#include "OtNodesFactory.h"


//
//	OtErodeHeightMapNode
//

class OtErodeHeightMapNode : public OtNodeClass {
public:
	// destructor
	~OtErodeHeightMapNode() {
		if (generating && future.valid()) {
			future.wait();
		}
	}

	// configure node
	inline void configure() override {
		addInputPin("World", heightMap);
		static constexpr const char* outputLabel = "Height Map";

		addOutputPin(outputLabel, erodedHeightMap)->addCustomRenderer([this](float width) {
			if (generating) {
				ImGui::SetNextItemWidth(width);
				ImGui::ProgressBar(static_cast<float>(-ImGui::GetTime()), ImVec2(), "Generating...");

			} else {
				OtUi::hSpacer(width - ImGui::CalcTextSize(outputLabel).x);
				OtUi::text(outputLabel);
			}

		}, OtUi::size(8.0f));
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		auto status = std::format("Drops: {}", run * dropPerRun);
		OtUi::centerTextInSpace(status.c_str(), itemWidth);

		if (generating) {
			if (OtUi::centeredButton("Stop", itemWidth)) {
				generating = false;
			}

		} else {
			if (!heightMap.isValid()) {
				ImGui::BeginDisabled();
			}

			if (OtUi::centeredButton("Start", itemWidth)) {
				run = 0;
				generating = true;
				workHeightMap = heightMap.clone();
				inputVersion = heightMap.getVersion();
				scheduleGeneration();
			}

			if (!heightMap.isValid()) {
				ImGui::EndDisabled();
			}
		}

		return false;
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::size(6.0f);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() * 2.0f;
	}

	// update node status
	inline bool onUpdate() override {
		if (generating) {
			if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				future.get();
				erodedHeightMap = workHeightMap;
				run++;

				if (heightMap.isValid()) {
					if (heightMap.getVersion() != inputVersion) {
						run = 0;
						workHeightMap = heightMap.clone();
						inputVersion = heightMap.getVersion();
					}

					scheduleGeneration();

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
		if (!heightMap.isValid()) {
			erodedHeightMap.clear();
		}
	}

	static constexpr const char* nodeName = "Erode Height Map";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::heightmap;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int run = 0;
	int dropPerRun = 1000;
	int inputVersion;

	// world component
	OtHeightMap heightMap;
	OtHeightMap workHeightMap;
	OtHeightMap erodedHeightMap;

	// work variables
	std::future<void> future;
	bool generating = false;

	// local functions
	void scheduleGeneration() {
		future = OtThreadPool::submit<void>([this]() {
			workHeightMap.erode(run, dropPerRun);
		});
	}
};


static OtNodesFactoryRegister<OtErodeHeightMapNode> registration;
