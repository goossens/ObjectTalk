//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <atomic>

#include "ImGuiPiano.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtMidiFile.h"
#include "OtMidiFileAsset.h"


//
//	OtMidiInputCircuit
//

class OtMidiInputCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		pitchOutput = addOutputPin("Pitch", OtCircuitPinClass::Type::control);
		gateOutput = addOutputPin("Gate", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		bool changed = false;
		ImGui::SetNextItemWidth(itemWidth - ImGui::CalcTextSize(" Midi File").x);

		if (asset.renderUI(" Midi File")) {
			if (asset.isNull()) {
				midifile.clear();

			} else if (asset.isReady()) {
				midifile = asset->getMidiFile();

			} else {
				midifile.clear();
			}

			changed |= true;
		}

		if (!asset.isReady()) {
			ImGui_PianoKeyboard(
				"Piano",
				ImVec2(itemWidth, keyboardHeight),
				&currentNote,
				36, 83,
				[](void* data, int msg, int key, float velocity) {
					OtMidiInputCircuit* circuit = (OtMidiInputCircuit*) data;

					if (key >= 128) {
						return false;
					}

					if (msg == NoteGetStatus) {
						return circuit->keyPressed[key];
					}

					if (msg == NoteOn) {
						circuit->keyPressed[key] = true;
						circuit->gate = true;
						circuit->pitch = OtAudioUtilities::midiNoteToPitch(key);
						circuit->velocity = velocity;
					}

					if (msg == NoteOff) {
						circuit->keyPressed[key] = false;
						circuit->gate = false;
						circuit->noteOff = true;
						circuit->velocity = 0.0f;
					}

					return false;
				}, this);
		}

		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return keyboardWidth;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + (asset.isReady() ? 0.0f : (keyboardHeight + ImGui::GetStyle().ItemSpacing.y));
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["midiFile"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtAssetDeserialize(data, "midiFile", basedir);
	}

	// generate samples
	void execute() override {
		// handle status changes for MIDI file
		if ((asset.isNull() || asset.isMissing() || asset.isInvalid()) && midifile.isValid()) {
			midifile.clear();

		} else if (asset.isReady() && asset->getMidiFile() != midifile) {
			midifile = asset->getMidiFile();
			midifile.loop();
		}

		// process events from the MIDI file (if it is running)
		if (midifile.isValid() && midifile.isPlaying()) {
			midifile.process([&](bool on, int note, int velocity) {
				gate = on;
				noteOff = noteOff | !on;
				pitch = OtAudioUtilities::midiNoteToPitch(note);
				velocity = static_cast<float>(velocity) / 128.0f;
			});
		}

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
	OtAsset<OtMidiFileAsset> asset;

	// work variables
	OtMidiFile midifile;
	bool keyPressed[128];
	int currentNote = 0;

	std::atomic<float> pitch = 500.0f;
	std::atomic<float> velocity = 0.0f;
	std::atomic<bool> gate = false;
	std::atomic<bool> noteOff = false;

	OtCircuitPin pitchOutput;
	OtCircuitPin gateOutput;
};

static OtCircuitFactoryRegister<OtMidiInputCircuit> registration;
