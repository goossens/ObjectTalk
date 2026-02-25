//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <mutex>
#include <string>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"
#include "OtFontAudio.h"
#include "OtUi.h"

#include "OtAudioSettings.h"
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"
#include "OtSampleBuffer.h"
#include "OtSampleFileAsset.h"


//
//	OtSampleInputCircuit
//

class OtSampleInputCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::PushItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 6.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		std::lock_guard<std::mutex> guard(mutex);
		auto changed = asset.renderUI("Source");

		if (!(audioOutput->isDestinationConnected() && asset.isReady())) {
			playing = false;
			pausing = false;
			ImGui::BeginDisabled();
		}

		if (OtAudioUi::audioLatchButton(OtFontAudio::play, &playing)) {
			if (playing) {
				sampleSize = asset->getSampleFile().getSampleCount();
				sampleLocation = 0;
				pausing = false;
			}
		}

		ImGui::SameLine();

		if (OtAudioUi::audioLatchButton(OtFontAudio::repeat, &looping)) {
			if (looping && !playing && !pausing) {
				playing = true;
				sampleSize = asset->getSampleFile().getSampleCount();
				sampleLocation = 0;
			}
		}

		if (playing || pausing) {
			ImGui::SameLine();

			if (OtAudioUi::audioLatchButton(OtFontAudio::pause, &pausing)) {
				playing = !pausing;
			}
		}

		if (!(audioOutput->isDestinationConnected() && asset.isReady())) { ImGui::EndDisabled(); }

		ImGui::PopItemWidth();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() * 2.0f;
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["samples"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtAssetDeserialize(data, "samples", basedir);
	}

	// generate output by evaluating MIDI messages
	void execute() override {
		std::lock_guard<std::mutex> guard(mutex);

		if (audioOutput->isDestinationConnected() && asset->isReady() && playing) {
			float output[OtAudioSettings::bufferSize];
			auto& sampler = asset->getSampleFile();

			for (size_t i = 0; i < OtAudioSettings::bufferSize; i++) {
				if (playing) {
					output[i] = sampler.get(static_cast<float>(sampleLocation++));

					if (sampleLocation == sampleSize) {
						if (looping) {
							sampleLocation = 0;

						} else {
							playing = false;
						}
					}

				} else {
					output[i] = 0.0f;
				}
			}

			audioOutput->setSamples(output);

		} else {
			audioOutput->setSamples(0.0f);
		}
	}

	static constexpr const char* circuitName = "Sample Input";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::input;
	static constexpr float width = 300.0f;

private:
	// properties
	OtAsset<OtSampleFileAsset> asset;

	// work variables
	OtCircuitPin audioOutput;
	std::mutex mutex;
	bool playing = false;
	bool looping = false;
	bool pausing = false;
	size_t sampleSize;
	size_t sampleLocation;
};

static OtCircuitFactoryRegister<OtSampleInputCircuit> registration;
