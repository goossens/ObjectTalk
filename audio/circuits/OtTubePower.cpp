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

#include "OtSingleEndedEL84.h"
#include "OtSingleEnded6V6.h"
#include "OtPushPullEL34.h"
#include "OtPushPullEL84.h"
#include "OtPushPull6L6.h"


//
//	OtTubePowerCircuit
//

class OtTubePowerCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		audioInput = this->addInputPin("Input", OtCircuitPinClass::Type::mono);
		audioOutput = this->addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		static constexpr const char* tubeTypes[] = {
			"Single Ended EL84",
			"Single Ended 6V6",
			"Push-Pull EL34",
			"Push-Pull EL84",
			"Push-Pull 6L6"
		};

		static constexpr size_t tubeTypeCount = sizeof(tubeTypes) / sizeof(*tubeTypes);

		ImGui::SetNextItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 4.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		auto changed = OtUi::selectorEnum("Tube", &tube, tubeTypes, tubeTypeCount);

		changed |= OtAudioUi::knob("Presence", &presence, -15.0f, 15.0f, "%.1f"); ImGui::SameLine();
		changed |= OtAudioUi::knob("Feedback", &feedback, -0.4f, 1.0f, "%.1f"); ImGui::SameLine();
		changed |= OtAudioUi::knob("Volume", &volume, 0.0f, 2.0f, "%.1f");
		return changed;
	}


	inline float getCustomRenderingWidth() override {
		return OtAudioUi::knobWidth(3);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + OtAudioUi::knobHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["tube"] = tube;
		(*data)["presence"] = presence;
		(*data)["feedback"] = feedback;
		(*data)["volume"] = volume;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		tube = data->value("tube", Tube::tubePushPullEL34);
		presence = data->value("presence", 0.0f);
		feedback = data->value("feedback", 0.0f);
		volume = data->value("volume", 1.0f);
	}

	// run power amp for one buffer
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			float output[OtAudioSettings::bufferSize];

			if (audioInput->isSourceConnected()) {
				float input[OtAudioSettings::bufferSize];
				audioInput->getSamples(input);

				auto in = input;
				auto out = output;

				switch (tube) {
					case Tube::tubeSingleEndedEL84:
						singleEndedEL84.setPresence(presence);
						singleEndedEL84.setFeedback(feedback);
						singleEndedEL84.setVolume(volume);
						singleEndedEL84.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tubeSingleEnded6V6:
						singleEnded6V6.setPresence(presence);
						singleEnded6V6.setFeedback(feedback);
						singleEnded6V6.setVolume(volume);
						singleEnded6V6.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tubePushPullEL34:
						pushPullEL34.setPresence(presence);
						pushPullEL34.setFeedback(feedback);
						pushPullEL34.setVolume(volume);
						pushPullEL34.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tubePushPullEL84:
						pushPullEL84.setPresence(presence);
						pushPullEL84.setFeedback(feedback);
						pushPullEL84.setVolume(volume);
						pushPullEL84.compute(OtAudioSettings::bufferSize, &in, &out);
						break;

					case Tube::tubePushPull6L6:
						pushPull6L6.setPresence(presence);
						pushPull6L6.setFeedback(feedback);
						pushPull6L6.setVolume(volume);
						pushPull6L6.compute(OtAudioSettings::bufferSize, &in, &out);
						break;
				}

			} else {
				std::fill(output, output + OtAudioSettings::bufferSize, 0.0f);
			}

			audioOutput->setSamples(output);
		}
	}

	static constexpr const char* circuitName = "Tube Power Amp";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::processor;

private:
	// properties
	enum class Tube {
		tubeSingleEndedEL84,
		tubeSingleEnded6V6,
		tubePushPullEL34,
		tubePushPullEL84,
		tubePushPull6L6
	};

	Tube tube = Tube::tubePushPullEL34;
	float presence = 0.0f;
	float feedback = 0.0f;
	float volume = 1.0f;

	// work variables
	OtCircuitPin audioInput;
	OtCircuitPin audioOutput;

	OtSingleEndedEL84 singleEndedEL84;
	OtSingleEnded6V6 singleEnded6V6;
	OtPushPullEL34 pushPullEL34;
	OtPushPullEL84 pushPullEL84;
	OtPushPull6L6 pushPull6L6;
};

static OtCircuitFactoryRegister<OtTubePowerCircuit> registration;
