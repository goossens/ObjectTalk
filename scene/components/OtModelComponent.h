//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//


#include <filesystem>

#include "nlohmann/json_fwd.hpp"

#include "OtModel.h"


//
//	OtModelComponent
//

class OtModelComponent {
public:
	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// update the model
	void update();

	// component name
	static constexpr char const* name = "Model";

	// stored properties
	std::filesystem::path modelPath;

	// runtime properties
	OtModel model;
};
