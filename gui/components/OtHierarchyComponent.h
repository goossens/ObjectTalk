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
#include "OtEntity.h"


//
//	OtHierarchyComponent
//

class OtHierarchyComponent : public OtComponent {
public:
	// properties
	OtEntity parent{OtEntityNull};
	OtEntity firstChild{OtEntityNull};
	OtEntity lastChild{OtEntityNull};
	OtEntity previousSibling{OtEntityNull};
	OtEntity nextSibling{OtEntityNull};

	nlohmann::json serialize() override;
	void deserialize(nlohmann::json data) override;
};
