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
#include "nlohmann/json.hpp"

#include "OtGlm.h"
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
		// render button
		if (ImGui::Button("Edit", ImVec2(itemWidth, 0.0f))) {
			ImGui::OpenPopup("ErosionPopup");
		}

		// open popup (if required)
		if (ImGui::BeginPopup("ErosionPopup")) {
			auto old = serialize().dump();
			bool changed = false;

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

			if (changed) {
				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}

			ImGui::EndChild();
			ImGui::EndPopup();
		}
	}

	inline float getCustomRenderingWidth() override {
		return ImGui::CalcTextSize("X").x * 8.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		auto heightData = nlohmann::json::object();
		heightData["octaves"] = heightConfig.octaves;
		heightData["gain"] = heightConfig.gain;
		heightData["lacunarity"] = heightConfig.lacunarity;
		heightData["amplitude"] = heightConfig.amplitude;
		heightData["frequency"] = heightConfig.frequency;
		heightData["offset"] = heightConfig.offset;

		auto erosionData = nlohmann::json::object();
		erosionData["scale"] = erosionConfig.scale;
		erosionData["strength"] = erosionConfig.strength;
		erosionData["gullyWeight"] = erosionConfig.gullyWeight;
		erosionData["detail"] = erosionConfig.detail;

		(*data)["height"] = heightData;
		(*data)["erosion"] = erosionData;
	}

	void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		if (data->contains("height")) {
			auto& heightData = (*data)["height"];
			heightConfig.octaves = heightData.value("octaves", 3);
			heightConfig.gain = heightData.value("gain", 0.1f);
			heightConfig.lacunarity = heightData.value("lacunarity", 2.0f);
			heightConfig.amplitude = heightData.value("amplitude", 0.125f);
			heightConfig.frequency = heightData.value("frequency", 3.0f);
			heightConfig.offset = heightData.value("offset", glm::vec2(-0.65f, 0.0f));
	}

		if (data->contains("erosion")) {
			auto erosionData = (*data)["erosion"];
			erosionConfig.scale = erosionData.value("scale", 0.15f);
			erosionConfig.strength = erosionData.value("strength", 0.22f);
			erosionConfig.gullyWeight = erosionData.value("gullyWeight", 0.5f);
			erosionConfig.detail = erosionData.value("detail", 1.5f);
		}
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
