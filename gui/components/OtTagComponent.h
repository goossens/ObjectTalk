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
//	OtTagComponent
//

class OtTagComponent : public OtComponent {
public:
	// constructors
	OtTagComponent() = default;
	OtTagComponent(const std::string& t) : tag(t) {}

	// GUI to change component properties
	bool renderGUI() override;

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// component name
	static constexpr char const* name = "Tag";

	// properties
	std::string tag;
};
