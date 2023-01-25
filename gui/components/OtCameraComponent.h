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

#include "OtCamera2.h"
#include "OtComponent.h"


//
//	OtCameraComponent
//

class OtCameraComponent : public OtComponent {
public:
	// constructors
	OtCameraComponent();
	OtCameraComponent(const std::string& type);

	// update the camera instance after type property change
	void updateCamera();

	// GUI to change component properties
	bool renderGUI() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// component name
	static constexpr char const* name = "Camera";

	// properties
	std::string type = "Perspective";
	OtCamera2 camera;
};
