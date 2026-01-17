//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtFontAudio.h"
#include "OtUi.h"

#include "OtMidi.h"


//
//	Constants
//

static constexpr float keyboardWidth = 300.0f;
static constexpr float keyboardHeight = 40.0f;


//
//	OtMidi::renderUI
//

bool OtMidi::renderUI() {
	// input types
	static constexpr const char* MidiSources[] = {
		"Keyboard",
		"MIDI File",
		"Device"
	};

	static constexpr size_t MidiSourceCount = sizeof(MidiSources) / sizeof(*MidiSources);

	bool changed = OtUi::selectorEnum("MIDI Source", &midiSource, MidiSources, MidiSourceCount);

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
					OtMidi* midi = (OtMidi*) data;

					if (key >= 128) {
						return false;
					}

					if (msg == NoteGetStatus) {
						return midi->keyPressed[key];
					}

					if (msg == NoteOn) {
						midi->keyPressed[key] = true;
						auto message = std::make_shared<OtMidiMessage>();
						message->makeNoteOn(1, key, static_cast<int>(velocity * 128.0f));
						std::lock_guard<std::mutex> guard(midi->mutex);
						midi->messages.emplace_back(std::move(message));
					}

					if (msg == NoteOff) {
						midi->keyPressed[key] = false;
						auto message = std::make_shared<OtMidiMessage>();
						message->makeNoteOff(1, key, static_cast<int>(velocity * 128.0f));
						std::lock_guard<std::mutex> guard(midi->mutex);
						midi->messages.emplace_back(std::move(message));
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

	return changed;
}


//
//	OtMidi::getLabelWidth

float OtMidi::getLabelWidth() {
	return ImGui::CalcTextSize("X").x * 11.0f + ImGui::GetStyle().ItemInnerSpacing.x;
}


//
//	OtMidi::getRenderWidth
//

float OtMidi::getRenderWidth() {
	return keyboardWidth;
}


//
//	OtMidi::getRenderHeight
//

float OtMidi::getRenderHeight() {
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



//
//	OtMidi::serialize
//

void OtMidi::serialize(nlohmann::json* data, std::string* basedir) {
	(*data)["midiSource"] = midiSource;
	(*data)["midiFile"] = OtAssetSerialize(asset.getPath(), basedir);
}


//
//	OtMidi::deserialize
//

void OtMidi::deserialize(nlohmann::json* data, std::string* basedir) {
	midiSource = data->value("midiSource", MidiSource::keyboard);
	asset = OtAssetDeserialize(data, "midiFile", basedir);
}


//
//	OtMidi::processEvents
//

void OtMidi::processEvents(std::function<void(std::shared_ptr<OtMidiMessage>)> callback) {
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

			midifile.process([&](std::shared_ptr<OtMidiMessage> message) {
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
							auto msg = std::make_shared<OtMidiMessage>();
							msg->makeNoteOff(message->getChannel(), i, 0);
							callback(msg);
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
