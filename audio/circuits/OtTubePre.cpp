//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioSettings.h"
#include "OtAudioUi.h"
#include "OtCircuitFactory.h"

#include "OtTubePre12AT7.h"
#include "OtTubePre12AU7.h"
#include "OtTubePre12AX7.h"
#include "OtTubePre12AY7.h"


//
//	OtTubePreCircuit
//

class OtTubePreCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		static constexpr const char* tubeTypes[] = {
			"12AT7",
			"12AU7",
			"12AX7",
			"12AY7"
		};

		static constexpr size_t tubeTypeCount = sizeof(tubeTypes) / sizeof(*tubeTypes);

		ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		auto changed = OtUi::selectorEnum("Tube", &tube, tubeTypes, tubeTypeCount);

		changed |= OtUi::knob("Gain", &gain, -20.0f, 20.0f, "%.1fdB"); ImGui::SameLine();
		changed |= OtUi::knob("Volume", &volume, -20.0f, 20.0f, "%.1fdB");
		return changed;
	}


	inline float getCustomRenderingWidth() override {
		return OtUi::knobWidth(2);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + OtUi::knobHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["tube"] = tube;
		(*data)["gain"] = gain;
		(*data)["middle"] = volume;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		tube = data->value("tube", Tube::tube12ax7);
		gain = data->value("gain", 0.0f);
		volume = data->value("volume", 0.0f);
	}

	// run preamp for one buffer
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			float output[OtAudioSettings::bufferSize];

			if (audioInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				audioInput->getSamples(input);

				auto in = input;
				auto out = output;

				switch (tube) {
					case Tube::tube12at7:
						tube12at7.setGain(gain);
						tube12at7.setVolume(volume);
						tube12at7.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tube12au7:
						tube12au7.setGain(gain);
						tube12au7.setVolume(volume);
						tube12au7.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tube12ax7:
						tube12ax7.setGain(gain);
						tube12ax7.setVolume(volume);
						tube12ax7.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tube12ay7:
						tube12ay7.setGain(gain);
						tube12ay7.setVolume(volume);
						tube12ay7.compute(OtAudioSettings::bufferSize, &in, &out);
						break;
				}

			} else {
				std::fill(output, output + OtAudioSettings::bufferSize, 0.0f);
			}

			audioOutput->setSamples(output);
		}
	}

	static constexpr const char* circuitName = "Tube Preamp";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	// properties
	enum class Tube {
		tube12at7,
		tube12au7,
		tube12ax7,
		tube12ay7
	};

	Tube tube = Tube::tube12ax7;
	float gain = 0.5f;
	float volume = 0.0f;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;

	OtTubePre12AT7 tube12at7;
	OtTubePre12AU7 tube12au7;
	OtTubePre12AX7 tube12ax7;
	OtTubePre12AY7 tube12ay7;
};

static OtCircuitFactoryRegister<OtTubePreCircuit> registration;
