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
//	OtIdComponent
//

class OtIdComponent : public OtComponent {
public:
	// constructors
	OtIdComponent(uint64_t i) : id(i) {}

	// properties
	uint64_t id;
};
