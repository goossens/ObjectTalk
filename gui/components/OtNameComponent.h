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

#include "OtComponent.h"


//
//	OtNameComponent
//

class OtNameComponent : public OtComponent {
public:
	// constructors
	OtNameComponent() = default;
	OtNameComponent(const std::string& n) : name(n) {}

	// properties
	std::string name;

	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;
};
