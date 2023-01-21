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

#include "OtComponent.h"


//
//	OtUuidComponent
//

class OtUuidComponent : public OtComponent {
public:
	// constructors
	OtUuidComponent() = default;
	OtUuidComponent(uint32_t u) : uuid(u) {}

	// properties
	uint32_t uuid;

	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;
};
