//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtTextureAsset.h"


//
//	OtParticleSettings
//

struct OtParticleSettings {
	// UI to change material properties
	bool renderUI();

	// (de)serialize material
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// enums
	enum {
		pointShape,
		sphereShape,
		hemisphereShape,
		circleShape
	};

	static constexpr const char* shapeTypes[] = {
		"Point",
		"Sphere",
		"Hemisphere",
		"Circle"
	};

	static constexpr size_t shapeTypesCount = sizeof(shapeTypes) / sizeof(*shapeTypes);

	enum {
		upDirection,
		outwardDirection
	};

	static constexpr const char* directionTypes[] = {
		"Upwards",
		"Outwards"
	};

	static constexpr size_t directionTypesCount = sizeof(directionTypes) / sizeof(*directionTypes);

	// properties
	int particles = 1024;
	float lifeSpan[2] = {2.0f, 4.0f};
	OtAsset<OtTextureAsset> atlas;
	int atlasRows = 1;
	int atlasColumns = 1;
	int shape = circleShape;
	int direction = upDirection;
	float velocity = 3.0f;
	float upVariation = 0.1f;
	float gravity = 0.5f;
	float rotation[2] = {0.0f, 0.0f};
	float scale[2] = {1.0f, 1.0f};
	float alpha[2] = {1.0f, 1.0f};

	// runtime information (updated every frame)
	glm::vec3 cameraPosition;
	float deltatime;
};
