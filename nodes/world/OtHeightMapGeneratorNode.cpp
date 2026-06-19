//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <future>

#include "imgui.h"

#include "OtThreadPool.h"
#include "OtUi.h"

#include "OtErosion.h"
#include "OtHeightMap.h"

#include "OtNodesFactory.h"


//
//	OtHeightMapGeneratorNode
//

class OtHeightMapGeneratorNode : public OtNodeClass {
public:
	// destructor
	~OtHeightMapGeneratorNode() {
		if (generating && future.valid()) {
			future.wait();
		}
	}

	// configure node
	inline void configure() override {
		addOutputPin("Height Map", heightMap);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		bool changed = false;

		// render button
		if (ImGui::Button("Edit", ImVec2(itemWidth, 0.0f))) {
			ImGui::OpenPopup("ErosionPopup");
		}

		// open popup (if required)
		if (ImGui::BeginPopup("ErosionPopup")) {
			auto size = ImVec2(
				OtUi::size(16.0f),
				ImGui::GetFrameHeightWithSpacing() * 7);

			ImGui::BeginChild("Heights", size, ImGuiChildFlags_Borders);
			OtUi::header("Heights");

			changed |= OtUi::selectorPowerOfTwo("Size", &heightSize, 64, 2048);
			changed |= OtUi::dragInt("Octaves", &heightConfig.octaves, 1, 10);
			changed |= OtUi::dragFloat("Gain", &heightConfig.gain, 0.1f, 1.0f);
			changed |= OtUi::dragFloat("Lacunarity", &heightConfig.lacunarity, 1, 10);
			changed |= OtUi::dragFloat("Amplitude", &heightConfig.amplitude, 0.1f, 1.0f);
			changed |= OtUi::dragFloat("Frequency", &heightConfig.frequency, 2, 20);

			// glm::vec2 offset{-0.65f, 0.0f};

			ImGui::EndChild();
			ImGui::SameLine();

			ImGui::BeginChild("Erosion", size, ImGuiChildFlags_Borders);
			OtUi::header("Erosion");

			changed |= OtUi::dragFloat("Scale", &erosionConfig.scale, 0.1f, 1.0f);
			changed |= OtUi::dragFloat("Strength", &erosionConfig.strength, 0.1f, 1.0f);
			changed |= OtUi::dragFloat("Gully Weight", &erosionConfig.gullyWeight, 0.1f, 1.0f);
			changed |= OtUi::dragFloat("Detail", &erosionConfig.detail, 0.1f, 5.0f);

			ImGui::EndChild();
			ImGui::EndPopup();
		}

		if (changed) {
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return ImGui::CalcTextSize("X").x * 8.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// update node status
	inline bool onUpdate() override {
		if (generating) {
			if (future.valid() && future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
				future.get();
				heightMap = workHeightMap;

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

	// running the generator
	inline void onExecute() override {
		if (generating) {
			moreRequests = true;

		} else {
			generating = true;
			scheduleGeneration();
		}
	}

	static constexpr const char* nodeName = "Height Map Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::world;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int heightSize = 128;
	OtErosion::HeightConfig heightConfig;
	OtErosion::ErosionConfig erosionConfig;

	// work variables
	std::future<void> future;
	bool generating = false;
	bool moreRequests = false;

	OtErosion erosion;
	OtHeightMap workHeightMap;
	OtHeightMap heightMap;

	// local functions
	void scheduleGeneration() {
		future = OtThreadPool::submit<void>([this]() {
			// ensure output has the right configuration
			workHeightMap.update(heightSize, heightSize);

			// run the generator
			auto scale = 1.0f / static_cast<float>(heightSize);

			for (auto y = 0; y < heightSize; y++) {
				for (auto x = 0; x < heightSize; x++) {
					auto height = erosion.sample(glm::vec2(x * scale, y * scale), heightConfig, erosionConfig).x;
					workHeightMap.setElevation(x, y, height);
				}
			}

			workHeightMap.incrementVersion();
		});
	}
};

static OtNodesFactoryRegister<OtHeightMapGeneratorNode> registration;
