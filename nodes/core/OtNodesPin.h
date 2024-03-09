//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtGeometry.h"
#include "OtImage.h"
#include "OtInstances.h"
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
	glm::vec3,
	glm::vec4,
	OtImage,
	OtTexture,
	OtGeometry,
	OtInstances>;

enum {
	OtNodesPinBoolType,
	OtNodesPinIntType,
	OtNodesPinFloatType,
	OtNodesPinStringType,
	OtNodesPinVectorType,
	OtNodesPinColoeType,
	OtNodesPinImageType,
	OtNodesPinTextureType,
	OtNodesPinGeometryType,
	OtNodesPinInstancesType
};

static constexpr const char* OtNodesPinTypeNames[] = {
	"bool",
	"int",
	"float",
	"string",
	"vector",
	"color",
	"image",
	"texture",
	"geometry",
	"instances" };


//
//	OtNodesPin
//

class OtNodesPinClass;
using OtNodesPin = std::shared_ptr<OtNodesPinClass>;
typedef std::function<void(float)> OtNodesPinRenderer;

class OtNodesPinClass {
public:
	enum {
		inputPin,
		outputPin
	};

	// constructor
	inline OtNodesPinClass(const char* n, int d) : name(n), direction(d) {
		id = OtNodesGenerateID();
	}

	// destructor
	virtual inline ~OtNodesPinClass() {};

	// handle custom renderer
	inline void addRenderer(OtNodesPinRenderer renderer, float width=0.0f) {
		render = renderer;
		renderingWidth = width;
		hasRenderer = true;
	}

	// check status
	inline bool isInput() { return direction == inputPin; }
	inline bool isOutput() { return direction == outputPin; }
	inline bool isVarying() { return varying; }

	// handle connections
	virtual void connectToSource(OtNodesPin srcPin) = 0;
	virtual void disconnectFromSource() = 0;
	inline OtNodesPin getSource() { return sourcePin; }
	inline bool isSourceConnected() { return sourcePin != nullptr; }

	inline void connectToDestination(OtNodesPin destPin) { destinationPin = destPin; }
	inline void disconnectFromDestination() { destinationPin = nullptr; }
	inline OtNodesPin getDestination() { return destinationPin; }
	inline bool isDestinationConnected() { return destinationPin != nullptr; }

	// evaluate the pin
	virtual void evaluate() = 0;

	// (de)serialize
	nlohmann::json serialize(std::string* basedir=nullptr);
	void deserialize(nlohmann::json data, bool restoreIDs=true, std::string* basedir=nullptr);

	// get the pin type as a string
	inline std::string getTypeName() { return OtNodesPinTypeNames[type]; }

	// properties
	int type;
	uint32_t id;
	const char* name;
	int direction;
	OtNodeClass* node;
	bool needsEvaluating{false};

	bool varying{false};

	OtNodesPinRenderer render = [](float){};
	float renderingWidth{0.0f};
	bool hasRenderer{false};

	OtNodesPin sourcePin;
	OtNodesPin destinationPin;
};


//
//	OtNodesPinImpl
//

template <typename T>
class OtNodesPinImpl : public OtNodesPinClass {
public:
	// constructor
	inline OtNodesPinImpl(const char* n, int d, T* v) : OtNodesPinClass(n, d) {
		static_assert(OtTypeListIndexOf<T, OtNodesPinTypes>() != -1, "Data type not allowed for node pin");
		type = OtTypeListIndexOf<T, OtNodesPinTypes>();
		value = v;
		defaultValue = *value;
	}

	// handle connections
	inline void connectToSource(OtNodesPin srcPin) override {
		sourcePin = srcPin;
		source = srcPin ? std::dynamic_pointer_cast<OtNodesPinImpl<T>>(srcPin)->value : nullptr;
		needsEvaluating = true;
	}

	inline void disconnectFromSource() override {
		*value = defaultValue;
		sourcePin = nullptr;
		source = nullptr;
		needsEvaluating = true;
	}

	inline bool isConnected() { return sourcePin != nullptr; }

	// evaluate the pin
	void evaluate() override {
		if (source) {
			if (*value != *source) {
				*value = *source;
				needsEvaluating = true;
			}
		}
	}

	// properties
	T* value;
	T defaultValue;
	T* source = nullptr;
};

