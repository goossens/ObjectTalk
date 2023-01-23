//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponent.h"


//
//	OtGeometryComponent
//

class OtGeometryComponent : public OtComponent {
public:
	// constructors
	OtGeometryComponent() = default;

	// return component name
	static const char* getName() { return "Geometry"; }

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// properties
	bool active = true;
};
