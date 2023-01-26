//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

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

	// update the camera instance after type property change
	void updateCamera();

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize();
	void deserialize(nlohmann::json data);

	// component name
	static constexpr char const* name = "Camera";

	// properties
	std::string type = "Perspective";
	OtCamera2 camera;
};
