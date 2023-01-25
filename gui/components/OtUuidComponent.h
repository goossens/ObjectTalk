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
	OtUuidComponent();
	OtUuidComponent(uint32_t u) : uuid(u) {}

	// assign a new UUI to the component
	void assignNewUuid();

	// (de)serialize component
	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;

	// component name
	static constexpr char const* name = "UUID";

	// properties
	uint32_t uuid;
};
