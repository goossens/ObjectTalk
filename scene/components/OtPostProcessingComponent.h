//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"


//
//	OtPostProcessingComponent
//

class OtPostProcessingComponent {
public:
	enum class Tonemap {
		reinhardSimple,
		reinhardExtended,
		filmic,
		acesFilmic,
		uncharted2,
		lottes,
		uchimura
	};

	static constexpr const char* tonemapTypes[] = {
		"Reinhard Simple",
		"Reinhard Extended",
		"Filmic",
		"Aces Filmic",
		"Uncharted 2",
		"Lottes",
		"Uchimura"
	};

	static constexpr size_t tonemapTypeCount = sizeof(tonemapTypes) / sizeof(*tonemapTypes);

	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Post Processing";

	// stored properties
	bool fxaa = false;
	bool fog = false;
	float fogDensity = 0.1f;
	glm::vec3 fogColor = glm::vec3(0.4f, 0.5f, 0.8f);
	bool bloom = false;
	float bloomIntensity = 0.8f;
	bool godrays = false;
	float exposure = 1.0f;
	float contrast = 1.0f;
	Tonemap tonemap = Tonemap::acesFilmic;
};
