//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

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
		events.clear();
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
						auto event = std::make_unique<Event>();
						event->makeNoteOn(1, key, static_cast<int>(velocity * 128.0f));
						std::lock_guard<std::mutex> guard(midi->mutex);
						midi->events.emplace_back(std::move(event));
					}

					if (msg == NoteOff) {
						midi->keyPressed[key] = false;
						auto event = std::make_unique<Event>();
						event->makeNoteOff(1, key, static_cast<int>(velocity * 128.0f));
						std::lock_guard<std::mutex> guard(midi->mutex);
						midi->events.emplace_back(std::move(event));
					}

					return false;
				}, this);

			break;
		}

		case MidiSource::midiFile: {
			if (asset.renderUI("MIDI File")) {
				if (asset.isNull()) {
					midifile.clear();

				} else if (asset.isReady()) {
					midifile = asset->getMidiFile();

				} else {
					midifile.clear();
				}

				changed |= true;
				break;
			}
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
			height += ImGui::GetFrameHeightWithSpacing();
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

void OtMidi::processEvents(std::function<void(Event*)> callback) {
	switch (midiSource) {
		case MidiSource::keyboard: {
			std::lock_guard<std::mutex> guard(mutex);

			for (auto& event : events) {
				callback(event.get());
			}

			events.clear();
			break;
		}

		case MidiSource::midiFile: {
			break;
		}

		case MidiSource::device: {
			break;
		}
	}
}
