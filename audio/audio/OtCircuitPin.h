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
#include <functional>
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

typedef std::function<void(float)> OtCircuitPinRenderer;

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

	// handle custom renderer
	inline OtCircuitPin addCustomRenderer(OtCircuitPinRenderer renderer, float width=0.0f) {
		render = renderer;
		renderingWidth = width;
		hasRenderer = true;
		return shared_from_this();
	}

	// check status
	inline bool isInput() { return direction == Direction::input; }
	inline bool isOutput() { return direction == Direction::output; }

	// handle connections
	inline void connectToSource(OtCircuitPin srcPin) { sourcePin = srcPin; }
	inline void disconnectFromSource() { sourcePin = nullptr; }
	inline OtCircuitPin getSource() { return sourcePin; }
	inline std::shared_ptr<OtSignalBuffer> getSignalBuffer() { return sourcePin->buffer; }
	inline bool isSourceConnected() { return sourcePin != nullptr; }

	inline void connectToDestination(OtCircuitPin destPin) { destinationPin = destPin; }
	inline void disconnectFromDestination() { destinationPin = nullptr; }
	inline OtCircuitPin getDestination() { return destinationPin; }
	inline bool isDestinationConnected() { return destinationPin != nullptr; }

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
	OtCircuitClass* circuit;

	OtCircuitPinRenderer render = [](float){};
	float renderingWidth = 0.0f;
	bool hasRenderer = false;

	std::shared_ptr<OtSignalBuffer> buffer;

	OtCircuitPin sourcePin;
	OtCircuitPin destinationPin;
};
