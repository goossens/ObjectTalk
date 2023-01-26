//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "nlohmann/json_fwd.hpp"


//
//	OtGeometryComponent
//

class OtGeometryComponent {
public:
	// constructors
	OtGeometryComponent() = default;

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize();
	void deserialize(nlohmann::json data);

	// component name
	static constexpr char const* name = "Geometry";

	// properties
	bool active = true;
};
