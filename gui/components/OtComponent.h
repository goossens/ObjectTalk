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
//	OtComponent
//

class OtComponent {
public:
	// destructor
	virtual ~OtComponent() {}

	// GUI to change component properties (returns true if changes are made)
	virtual bool renderGUI() { return false; }

	// (de)serialize component
	virtual nlohmann::json serialize() = 0;
	virtual void deserialize(nlohmann::json data) = 0;

	std::string serializeToString();
	void deserializeFromString(const std::string& data);
};
