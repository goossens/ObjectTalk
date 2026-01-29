//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtAudioSettings.h"
#include "OtCircuitFactory.h"
#include "OtSynth.h"


//
//	OtPolyphonicSynth
//

class OtPolyphonicSynth : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		midiInput = addInputPin("MIDI", OtCircuitPinClass::Type::midi);
		audioOutput = addOutputPin("Output", OtCircuitPinClass::Type::mono);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		auto changed = false;

		if (ImGui::Button("Edit", ImVec2(itemWidth, 0.0f))) {
			ImGui::OpenPopup("synthEdit");
		}

		if (ImGui::BeginPopup("synthEdit")) {
			changed |= synth.renderUI(itemWidth);
			ImGui::EndPopup();
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return ImGui::CalcTextSize("X").x * 6.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		synth.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		synth.deserialize(data, basedir);
	}

	// process samples
	void execute() override {
		if (audioOutput->isDestinationConnected()) {
			if (midiInput->isSourceConnected()) {
				for (auto& message : midiInput->getMidiInputBuffer()) {
					synth.processMidiMessage(message);
				}

				float buffer[OtAudioSettings::bufferSize];
				synth.get(buffer, OtAudioSettings::bufferSize);
				audioOutput->setSamples(buffer);
			}
		}
	};

	static constexpr const char* circuitName = "Polyphonic Synth";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::generator;

private:
	// properties
	OtSynth synth;

	// work variables
	OtCircuitPin midiInput;
	OtCircuitPin audioOutput;
};

static OtCircuitFactoryRegister<OtPolyphonicSynth> registration;
