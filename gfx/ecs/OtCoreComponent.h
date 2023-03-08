//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <filesystem>
#include <string>

#include "nlohmann/json_fwd.hpp"

#include "OtEntity.h"


//
//	OtCoreComponent
//

class OtCoreComponent {
public:
	// constructors
	OtCoreComponent();
	OtCoreComponent(uint32_t uuid, const std::string& tag);

	// assign a new UUI to the component
	void assignNewUuid();

	// GUI to change component properties
	bool renderGUI();

	// (de)serialize component
	nlohmann::json serialize(std::filesystem::path* basedir);
	void deserialize(nlohmann::json data, std::filesystem::path* basedir);

	// component name
	static constexpr char const* name = "Core";

	// properties
	uint32_t uuid;
	std::string tag;

	OtEntity parent = OtEntityNull;
	OtEntity firstChild = OtEntityNull;
	OtEntity lastChild = OtEntityNull;
	OtEntity previousSibling = OtEntityNull;
	OtEntity nextSibling = OtEntityNull;
};
