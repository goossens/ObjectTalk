//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <array>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "ImGuiCurve.h"

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
	enum class Shape{
		point,
		circle,
		cone,
		hemisphere,
		sphere
	};

	static constexpr const char* shapeTypes[] = {
		"Point",
		"Circle",
		"Cone",
		"Hemisphere",
		"Sphere"
	};

	static constexpr size_t shapeTypesCount = sizeof(shapeTypes) / sizeof(*shapeTypes);

	// properties
	int particles = 1024;
	float lifeSpanLow = 1.0f;
	float lifeSpanHigh = 5.0f;
	OtAsset<OtTextureAsset> atlas;
	int atlasRows = 1;
	int atlasColumns = 1;
	Shape shape = Shape::circle;
	float speed = 0.5f;
	float gravity = 0.3f;
	float rotation[2] = {0.0f, 0.0f};
	std::array<ImVec2, 6> scale{ImVec2(ImGui::CurveTerminator, 0.0f)};
	std::array<ImVec2, 6> alpha{ImVec2(ImGui::CurveTerminator, 0.0f)};

	// IDE information
	int scaleSelection = -1;
	int alphaSelection = -1;

	// runtime information (updated every frame)
	glm::vec3 cameraPosition;
	float deltatime;
};
