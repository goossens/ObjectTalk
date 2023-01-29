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
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtCamera2.h"


//
//	OtCameraComponent
//

class OtCameraComponent {
public:
	// constructors
	OtCameraComponent();
	OtCameraComponent(const std::string& type);

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Camera";

	// stored properties
	OtCamera2 camera;

private:
	// create a new named camera instance
	void createCamera(const std::string& type);
};
