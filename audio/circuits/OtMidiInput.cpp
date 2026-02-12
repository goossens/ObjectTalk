//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>

#include "ImGuiPiano.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"
#include "OtFontAudio.h"
#include "OtUi.h"

#include "OtAudioUi.h"
#include "OtAudioUtilities.h"
#include "OtCircuitFactory.h"
#include "OtMidiBuffer.h"
#include "OtMidiFile.h"
#include "OtMidiFileAsset.h"
#include "OtMidiMessage.h"


//
//	OtMidiInputCircuit
//

class OtMidiInputCircuit : public OtCircuitClass {
public:
	// configure circuit
	inline void configure() override {
		midiOutput = addOutputPin("MIDI", OtCircuitPinClass::Type::midi);
		pitchOutput = addOutputPin("Freq", OtCircuitPinClass::Type::control);
		gateOutput = addOutputPin("Gate", OtCircuitPinClass::Type::control);
	}

	// render custom fields
	inline bool customRendering(float itemWidth) override {
		// input types
		static constexpr const char* midiSources[] = {
			"Keyboard",
			"MIDI File",
			"Device"
		};

		static constexpr size_t MidiSourceCount = sizeof(midiSources) / sizeof(*midiSources);

		ImGui::PushItemWidth(itemWidth - (ImGui::CalcTextSize("X").x * 11.0f + ImGui::GetStyle().ItemInnerSpacing.x));
		auto changed = OtUi::selectorEnum("MIDI Source", &midiSource, midiSources, MidiSourceCount);

		if (changed) {
			messages.clear();
		}

		switch (midiSource) {
			case MidiSource::keyboard: {
				ImGui::SetNextItemWidth(keyboardWidth);

				ImGui_PianoKeyboard(
					"Piano",
					ImVec2(keyboardWidth, keyboardHeight),
					&currentNote,
					36, 83,
					[](void* data, int msg, int key, float velocity) {
						OtMidiInputCircuit* midi = (OtMidiInputCircuit*) data;

						if (key >= 128) {
							return false;
						}

						if (msg == NoteGetStatus) {
							return midi->keyPressed[key];
						}

						if (msg == NoteOn) {
							midi->keyPressed[key] = true;
							std::lock_guard<std::mutex> guard(midi->mutex);
							midi->messages.emplace_back(OtMidiNoteOn(1, key, static_cast<int>(velocity * 127.0f)));
						}

						if (msg == NoteOff) {
							midi->keyPressed[key] = false;
							std::lock_guard<std::mutex> guard(midi->mutex);
							midi->messages.emplace_back(OtMidiNoteOff(1, key, static_cast<int>(velocity * 127.0f)));
						}

						return false;
					}, this);

				break;
			}

			case MidiSource::midiFile: {
				std::lock_guard<std::mutex> guard(mutex);

				if (asset.renderUI("MIDI File")) {
					midifile.clear();
					changed |= true;
					break;
				}

				if (asset.isReady() && midifile != asset->getMidiFile()) {
					midifile = asset->getMidiFile();
				}

				ImGui::PushFont(OtUi::getAudioFont(), 0.0f);
				if (!asset.isReady()) { ImGui::BeginDisabled(); }

				bool playing = midifile.isPlaying();
				bool looping = midifile.isLooping();
				bool pausing = midifile.isPausing();

				if (OtUi::latchButton(OtFontAudio::play, &playing)) {
					if (playing) {
						midifile.start();

					} else {
						midifile.stop();
					}
				}

				ImGui::SameLine();

				if (OtUi::latchButton(OtFontAudio::repeat, &looping)) {
					midifile.setLooping(looping);

					if (looping && !playing && !pausing) {
						midifile.start();
					}
				}

				if (playing || pausing) {
					ImGui::SameLine();

					if (OtUi::latchButton(OtFontAudio::pause, &pausing)) {
						if (pausing) {
							midifile.pause();

						} else {
							midifile.resume();
						}
					}
				}

				if (!asset.isReady()) { ImGui::EndDisabled(); }
				ImGui::PopFont();
			}

			case MidiSource::device:
				break;
		}

		ImGui::PopItemWidth();
		return changed;
	}

	inline float getCustomRenderingWidth() override {
		return keyboardWidth;
	}

	inline float getCustomRenderingHeight() override {
		auto height = ImGui::GetFrameHeightWithSpacing();

		switch (midiSource) {
			case MidiSource::keyboard:
				height += keyboardHeight + ImGui::GetStyle().ItemSpacing.y;
				break;

			case MidiSource::midiFile: {
				height += ImGui::GetFrameHeightWithSpacing() * 2.0f;
				break;
			}

			case MidiSource::device: {
				break;
			}
		}

		return height;
	}

	// (de)serialize circuit
	inline void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["midiSource"] = midiSource;
		(*data)["midiFile"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	inline void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		midiSource = data->value("midiSource", MidiSource::keyboard);
		asset = OtAssetDeserialize(data, "midiFile", basedir);
	}

	// process MIDI events from selected source
	void processEvents(std::function<void(OtMidiMessage)> callback) {
		switch (midiSource) {
			case MidiSource::keyboard: {
				std::lock_guard<std::mutex> guard(mutex);

				for (auto& message : messages) {
					callback(message);
				}

				messages.clear();
				break;
			}

			case MidiSource::midiFile: {
				std::lock_guard<std::mutex> guard(mutex);

				midifile.process([&](OtMidiMessage message) {
					if (message->isNoteOn()) {
						auto note = message->getKeyNumber();

						if (!keyPressed[note]) {
							keyPressed[note] = true;
							callback(message);
						}

					} else if (message->isNoteOff()) {
						auto note = message->getKeyNumber();

						if (keyPressed[note]) {
							keyPressed[note] = false;
							callback(message);
						}

					} else if (message->isAllNotesOff()) {
						for (size_t i = 0; i < 128; i++) {
							if (keyPressed[i]) {
								callback(OtMidiNoteOff(message->getChannel(), static_cast<int>(i), 0));
								keyPressed[i] = false;
							}
						}
					}
				});

				break;
			}

			case MidiSource::device: {
				break;
			}
		}
	}

	// generate output by evaluating MIDI messages
	void execute() override {
		bool noteOff = false;
		auto& midiBuffer = midiOutput->getMidiOutputBuffer();
		midiBuffer.clear();

		processEvents([&](OtMidiMessage message) {
			midiBuffer.emplace_back(message);

			if (message->isNoteOn()) {
				freq = OtAudioUtilities::midiNoteToPitch(message->getKeyNumber());
				velocity = message->getVelocity() / 128.0f;
				gate = true;

			} else if (message->isNoteOff()) {
				velocity = message->getVelocity() / 128.0f;
				gate = false;
				noteOff = true;
			}
		});

		// send freq signal (if connected)
		if (pitchOutput->isDestinationConnected()) {
			pitchOutput->setSamples(OtAudioUtilities::freqToCv(freq));

		} else {
			pitchOutput->setSamples(0.0f);
		}

		// send gate signal (if connected)
		if (gateOutput->isDestinationConnected()) {
			gateOutput->setSamples((gate && !noteOff) ? 1.0f : 0.0f);

		} else {
			gateOutput->setSamples(0.0f);
		}

		noteOff = false;
	}

	static constexpr const char* circuitName = "MIDI Input";
	static constexpr OtCircuitClass::Category circuitCategory = OtCircuitClass::Category::input;
	static constexpr float keyboardWidth = 300.0f;
	static constexpr float keyboardHeight = 40.0f;

private:
	// properties
	enum class MidiSource {
		keyboard,
		midiFile,
		device
	};

	MidiSource midiSource = MidiSource::keyboard;
	OtAsset<OtMidiFileAsset> asset;

	// work variables
	float freq = 440.0f;
	float velocity = 0.0f;
	bool gate = false;

	OtMidiFile midifile;

	std::array<bool, 128> keyPressed = {false};
	int currentNote = 0;
	OtMidiBuffer messages;

	std::mutex mutex;

	OtCircuitPin midiOutput;
	OtCircuitPin pitchOutput;
	OtCircuitPin gateOutput;
};

static OtCircuitFactoryRegister<OtMidiInputCircuit> registration;
