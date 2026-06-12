//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <format>

#include "OtHeightMap.h"
#include "OtThreadPool.h"
#include "OtUi.h"

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
		addOutputPin("Height Map", erodedHeightMap);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		auto status = std::format("Drops: {}", run * dropPerRun);
		OtUi::centerTextInSpace(status.c_str(), itemWidth);

		if (generating) {
			auto pos = ImGui::GetCursorScreenPos();

			if (ImGui::Button("Stop")) {
				generating = false;
			}

			OtUi::spinner(ImVec2(pos.x + itemWidth * 0.5f, pos.y), OtUi::size(1.0f));

		} else {
			if (!heightMap.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Start")) {
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
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::size(7.0f);
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
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::world;
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
