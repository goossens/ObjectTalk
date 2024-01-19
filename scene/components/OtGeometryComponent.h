//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtGeometry.h"


//
//	OtGeometryComponent
//

class OtGeometryComponent {
public:
	// constructors
	OtGeometryComponent();
	OtGeometryComponent(const std::string& type);

	// UI to change component properties
	bool renderUI();

	// (de)serialize component
	nlohmann::json serialize(std::string* basedir);
	void deserialize(nlohmann::json data, std::string* basedir);

	// component name
	static constexpr char const* name = "Geometry";

	// stored properties
	OtGeometry geometry;
	bool wireframe = false;
	bool transparent = false;
	bool cullback = true;

private:
	// create a named geometry instance
	void createGeometry(const std::string& type);
};
