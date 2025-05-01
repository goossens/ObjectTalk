//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"


//
//	OtMessageComponent
//

class OtMessageComponent {
public:
	// UI to change component properties
	bool renderUI();

	// update properties
	inline void show(const std::string& m, float v, float f) {
		message = m;
		visibleTime = v;
		fadeTime = f;

		visibleRemaining = v;
		fadeRemaining = f;
	}

	inline void hide() {
		visibleRemaining = -1.0f;
		fadeRemaining = -1.0f;
	}

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Message";

	// stored properties
	std::string message;
	float visibleTime = 0.0f;
	float fadeTime = 0.0f;

	// runtime properties
	float visibleRemaining;
	float fadeRemaining;
};
