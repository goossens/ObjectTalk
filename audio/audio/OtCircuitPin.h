//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtCircuitUtils.h"
#include "OtCircuitUtils.h"
#include "OtSignalBuffer.h"


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
	inline OtCircuitPinClass(const char* n, Type t, Direction d, bool a=false) : name(n), type(t), direction(d), attenuationFlag(a) {
		id = OtCircuitGenerateID();

		if (direction == Direction::output) {
			switch (type) {
				case Type::mono:
					buffer = std::make_shared<OtSignalBuffer>(1);
					break;

				case Type::stereo:
					buffer = std::make_shared<OtSignalBuffer>(2);
					break;

				case Type::control:
					buffer = std::make_shared<OtSignalBuffer>(1);
					break;
			}
		}
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
	inline float getSample(size_t sample) { return sourcePin->buffer->get(0, sample) * attenuation; }
	inline float getSample(size_t channel, size_t sample) { return sourcePin->buffer->get(channel, sample) * attenuation; }
	inline float* getSamples() { return sourcePin->buffer->data(); }

	inline void setSample(size_t sample, float value) { buffer->set(0, sample, value * attenuation); }
	inline void setSample(size_t channel, size_t sample, float value) { buffer->set(channel, sample, value * attenuation); }
	inline void setSamples(float value) { buffer->clear(value); }

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
	bool attenuationFlag;
	float attenuation = 1.0f;
	OtCircuitClass* circuit;

	std::shared_ptr<OtSignalBuffer> buffer;
	OtCircuitPin sourcePin;
	int destinationConnections = 0;
};
