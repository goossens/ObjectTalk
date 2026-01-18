//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "ImGuiPiano.h"
#include "nlohmann/json.hpp"

#include "OtAsset.h"

#include "OtAudioUtilities.h"
#include "OtMidiBuffer.h"
#include "OtMidiFile.h"
#include "OtMidiFileAsset.h"
#include "OtMidiMessage.h"


//
//	OtMidi
//

class OtMidi {
public:
	// UI to change properties
	bool renderUI();
	float getLabelWidth();
	float getRenderWidth();
	float getRenderHeight();

	// (de)serialize data
	virtual void serialize(nlohmann::json* data, std::string* basedir);
	virtual void deserialize(nlohmann::json* data, std::string* basedir);

	// process MIDI events
	void processEvents(std::function<void(OtMidiMessage)> callback);

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
	OtMidiFile midifile;

	std::array<bool, 128> keyPressed = {false};
	int currentNote = 0;
	OtMidiBuffer messages;

	std::mutex mutex;
};
