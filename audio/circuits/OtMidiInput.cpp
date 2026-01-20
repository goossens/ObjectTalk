//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ImGuiPiano.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtMidi.h"


//
//	OtMidiInputCircuit
//

class OtMidiInputCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		midiOutput = addOutputPin("MIDI", OtCircuitPinClass::Type::midi);
		pitchOutput = addOutputPin("Pitch", OtCircuitPinClass::Type::control)->hasTuning();
		gateOutput = addOutputPin("Gate", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		ImGui::PushItemWidth(itemWidth - midi.getLabelWidth());
		auto changed = midi.renderUI();
		ImGui::PopItemWidth();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return midi.getRenderWidth();
	}

	inline float getCustomRenderingHeight() override {
		return midi.getRenderHeight();
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		midi.serialize(data, basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		midi.deserialize(data, basedir);
	}

	// generate audio stream by evaluating MIDI messages
	void execute() override {
		auto& midiBuffer = midiOutput->getMidiOutputBuffer();
		midiBuffer.clear();

		midi.processEvents([&](OtMidiMessage message) {
			midiBuffer.emplace_back(message);

			if (message->isNoteOn()) {
				pitch = OtAudioUtilities::midiNoteToPitch(message->getKeyNumber());
				velocity = message->getVelocity() / 128.0f;
				gate = true;

			} else if (message->isNoteOff()) {
				velocity = message->getVelocity() / 128.0f;
				gate = false;
				noteOff |= true;
			}
		});

		// send pitch signal (if connected)
		if (pitchOutput->isDestinationConnected()) {
			pitchOutput->setSamples(OtAudioUtilities::pitchToCv(pitch));
		}

		// send gate signal (if connected)
		if (gateOutput->isDestinationConnected()) {
			gateOutput->setSamples(gate ? 1.0f : 0.0f);

			if (gate && noteOff) {
				gateOutput->setSample(0, 0.0f);
				noteOff = false;
			}
		}
	}

	static constexpr const char* circuitName = "MIDI Input";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::input;
	static constexpr float keyboardWidth = 300.0f;
	static constexpr float keyboardHeight = 40.0f;

private:
	// properties
	OtMidi midi;

	// work variables
	float pitch = 440.0f;
	float velocity = 0.0f;
	bool gate = false;
	bool noteOff = false;

	OtCircuitPin midiOutput;
	OtCircuitPin pitchOutput;
	OtCircuitPin gateOutput;
};

static OtCircuitFactoryRegister<OtMidiInputCircuit> registration;
