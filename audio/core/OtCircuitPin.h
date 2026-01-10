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

#include "nlohmann/json_fwd.hpp"

#include "OtAssert.h"

#include "OtCircuitUtils.h"
#include "OtCircuitUtils.h"
#include "OtAudioBuffer.h"


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
		control
	};

	enum class Direction {
		input,
		output
	};

	static constexpr const char* typeNames[] = {
		"mono",
		"stereo",
		"control"
	};

	// constructor
	inline OtCircuitPinClass(const char* n, Type t, Direction d) : name(n), type(t), direction(d) {
		id = OtCircuitGenerateID();

		if (direction == Direction::output) {
			switch (type) {
				case Type::mono:
					buffer = std::make_shared<OtAudioBuffer>(1);
					break;

				case Type::stereo:
					buffer = std::make_shared<OtAudioBuffer>(2);
					break;

				case Type::control:
					buffer = std::make_shared<OtAudioBuffer>(1);
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
	inline float* getSamples() { return sourcePin->buffer->data(); }

	void setSample(size_t channel, size_t sample, float value);
	inline void setSample(size_t sample, float value) { setSample(0, sample, value); }
	inline void setSamples(float value) { buffer->clear(value); }

	inline OtAudioBuffer& getInputBuffer() { return *(sourcePin->buffer); }
	inline OtAudioBuffer& getOutputBuffer() { return *buffer; }

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
	float tuningOctaves = 0.0f;
	float tuningSemitones = 0.0f;
	float tuningCents = 0.0f;
	OtCircuitClass* circuit;

	std::shared_ptr<OtAudioBuffer> buffer;
	OtCircuitPin sourcePin;
	int destinationConnections = 0;
};
