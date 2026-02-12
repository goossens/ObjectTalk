//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "nlohmann/json_fwd.hpp"

#include "OtAssert.h"

#include "OtCircuitUtils.h"
#include "OtCircuitUtils.h"
#include "OtAudioBuffer.h"
#include "OtMidiBuffer.h"


//
//	Forward definitions
//

class OtCircuitClass;


//
//	OtCircuitPin
//

class OtCircuitPinClass;
using OtCircuitPin = std::shared_ptr<OtCircuitPinClass>;

class OtCircuitPinClass : public std::enable_shared_from_this<OtCircuitPinClass> {
public:
	// pin types and direction
	enum class Type {
		mono,
		stereo,
		control,
		midi
	};

	enum class Direction {
		input,
		output
	};

	static constexpr const char* typeNames[] = {
		"mono",
		"stereo",
		"control",
		"midi"
	};

	// constructor
	inline OtCircuitPinClass(const char* n, Type t, Direction d) : name(n), type(t), direction(d) {
		id = OtCircuitGenerateID();

		if (direction == Direction::output) {
			switch (type) {
				case Type::mono:
					audioBuffer = std::make_shared<OtAudioBuffer>(1);
					break;

				case Type::stereo:
					audioBuffer = std::make_shared<OtAudioBuffer>(2);
					break;

				case Type::control:
					audioBuffer = std::make_shared<OtAudioBuffer>(1);
					break;

				case Type::midi:
					midiBuffer = std::make_shared<OtMidiBuffer>();
					break;
			}
		}
	}

	// set options
	inline OtCircuitPin hasAttenuation(bool flag=true) {
		attenuationFlag = flag;
		return shared_from_this();
	}

	inline OtCircuitPin hasTuning(bool flag=true) {
		tuningFlag = flag;
		return shared_from_this();
	}

	// check status
	inline bool isInput() { return direction == Direction::input; }
	inline bool isOutput() { return direction == Direction::output; }

	// handle connections
	inline void connectToSource(OtCircuitPin srcPin) { sourcePin = srcPin; }
	inline void disconnectFromSource() { sourcePin = nullptr; }
	inline OtCircuitPin getSource() { return sourcePin; }
	inline bool isSourceConnected() { return sourcePin != nullptr; }

	inline void connectToDestination() { destinationConnections++; }
	inline void disconnectFromDestination() { destinationConnections--; }
	inline bool isDestinationConnected() { return destinationConnections != 0; }

	// access incoming and outgoing samples
	float getSample(size_t channel, size_t sample);
	inline float getSample(size_t sample) { return getSample(0, sample); }
	void getSamples(float* samples);

	void setSample(size_t channel, size_t sample, float value);
	inline void setSample(size_t sample, float value) { setSample(0, sample, value); }
	void setSamples(float* value);
	void setSamples(float value);

	inline OtMidiBuffer& getMidiInputBuffer() { return *(sourcePin->midiBuffer); }
	inline OtMidiBuffer& getMidiOutputBuffer() { return *midiBuffer; }

	// (de)serialize
	nlohmann::json serialize(std::string* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);

	// get the pin type as a string
	inline std::string getTypeName() { return typeNames[static_cast<size_t>(type)]; }

	// properties
	uint32_t id;
	const char* name;
	Type type;
	Direction direction;
	bool attenuationFlag = false;
	float attenuation = 1.0f;
	bool tuningFlag = false;
	float tuning = 0.0f;
	OtCircuitClass* circuit;

	std::shared_ptr<OtAudioBuffer> audioBuffer;
	std::shared_ptr<OtMidiBuffer> midiBuffer;
	OtCircuitPin sourcePin;
	int destinationConnections = 0;
};
