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

#include "OtGeometry.h"


//
//	OtGeometryComponent
//

class OtGeometryComponent {
public:
	// constructors
	OtGeometryComponent();
	OtGeometryComponent(const std::string& type);

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Geometry";

	// stored properties
	OtGeometry geometry;
	bool wireframe = false;
	bool cullback = true;

private:
	// create a new named geometry instance
	void createGeometry(const std::string& type);
};
