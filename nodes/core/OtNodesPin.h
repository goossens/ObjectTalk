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

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtTypeList.h"

#include "OtColor.h"
#include "OtFont.h"
#include "OtGeometry.h"
#include "OtImage.h"
#include "OtInstances.h"
#include "OtManifold.h"
#include "OtMap.h"
#include "OtShape.h"
#include "OtTexture.h"

#include "OtNodesUtils.h"


//
//	Forward definitions
//

class OtNodeClass;


//
//	Data types
//

using OtNodesPinTypes = OtTypeList<
	bool,
	int,
	float,
	std::string,
	glm::vec2,
	glm::vec3,
	glm::vec4,
	OtColor,
	OtFont,
	OtImage,
	OtTexture,
	OtShape,
	OtGeometry,
	OtManifold,
	OtMap,
	OtInstances>;

static constexpr const char* OtNodesPinTypeNames[] = {
	"bool",
	"int",
	"float",
	"string",
	"vector2",
	"vector3",
	"vector4",
	"color",
	"font",
	"image",
	"texture",
	"shape",
	"geometry",
	"manifold",
	"map",
	"instances"
};


//
//	OtNodesPin
//

class OtNodesPinClass;
using OtNodesPin = std::shared_ptr<OtNodesPinClass>;

struct OtNodesPinInputConfig {
	std::function<void(nlohmann::json*)> serializeValue;
	std::function<void(nlohmann::json*)> deserializeValue;
	std::function<float()> getMinimumSize;
	std::function<void(OtNodesPin, float)> editValue;
};

typedef std::function<void(float)> OtNodesPinRenderer;

class OtNodesPinClass : public std::enable_shared_from_this<OtNodesPinClass> {
public:
	// pin types
	enum class Direction {
		input,
		output
	};

	// constructor
	inline OtNodesPinClass(const char* n, Direction d) : name(n), direction(d) {
		id = OtNodesGenerateID();
	}

	// destructor
	virtual inline ~OtNodesPinClass() {
		if (inputConfig) {
			delete inputConfig;
		}
	};

	// handle custom renderer
	inline OtNodesPin addCustomRenderer(OtNodesPinRenderer renderer, float width=0.0f) {
		render = renderer;
		renderingWidth = width;
		hasRenderer = true;
		return shared_from_this();
	}

	// check status
	inline bool isInput() { return direction == Direction::input; }
	inline bool isOutput() { return direction == Direction::output; }
	inline bool isVarying() { return varying; }

	// handle connections
	virtual void connectToSource(OtNodesPin srcPin) = 0;
	virtual void disconnectFromSource() = 0;
	inline OtNodesPin getSource() { return sourcePin; }
	inline bool isSourceConnected() { return sourcePin != nullptr; }

	inline void connectToDestination() { destinationConnections++; }
	inline void disconnectFromDestination() { destinationConnections--; }
	inline bool isDestinationConnected() { return destinationConnections != 0; }

	// process the input and see if value has changed
	virtual bool processInput() { return false; };

	// (de)serialize
	nlohmann::json serialize(std::string* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);

	// get the pin type as a string
	inline std::string getTypeName() { return OtNodesPinTypeNames[type]; }

	// properties
	int type;
	uint32_t id;
	const char* name;
	Direction direction;
	OtNodeClass* node;

	bool varying = false;

	OtNodesPinInputConfig* inputConfig = nullptr;
	OtNodesPinRenderer render = [](float){};
	float renderingWidth = 0.0f;
	bool hasRenderer = false;

	OtNodesPin sourcePin;
	int destinationConnections = 0;
};


//
//	OtNodesPinInputConfig
//

OtNodesPinInputConfig* OtNodesPinCreateInputConfig(bool& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(int& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(float& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(std::string& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec2& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec3& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(glm::vec4& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtColor& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtFont& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtImage& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtTexture& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtShape& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtGeometry& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtManifold& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtMap& value);
OtNodesPinInputConfig* OtNodesPinCreateInputConfig(OtInstances& value);


//
//	OtNodesPinImpl
//

template <typename T>
class OtNodesPinImpl : public OtNodesPinClass {
public:
	// constructor
	inline OtNodesPinImpl(const char* n, Direction d, T& v) : OtNodesPinClass(n, d) {
		static_assert(OtTypeListIndexOf<T, OtNodesPinTypes>() != -1, "Data type not allowed for node pin");
		type = OtTypeListIndexOf<T, OtNodesPinTypes>();
		value = &v;
		defaultValue = *value;
		inputConfig = OtNodesPinCreateInputConfig(v);
	}

	// handle connections
	inline void connectToSource(OtNodesPin srcPin) override {
		sourcePin = srcPin;
		source = srcPin ? std::dynamic_pointer_cast<OtNodesPinImpl<T>>(srcPin)->value : nullptr;
	}

	inline void disconnectFromSource() override {
		*value = defaultValue;
		sourcePin = nullptr;
		source = nullptr;
	}

	inline bool isConnected() { return sourcePin != nullptr; }

	// process the input and see if value has changed
	bool processInput() override {
		if (source && *value != *source) {
			*value = *source;
			return true;

		} else {
			return false;
		}
	}

	// properties
	T* value;
	T defaultValue;
	T* source = nullptr;
};
