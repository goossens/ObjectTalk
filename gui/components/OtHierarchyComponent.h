//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "entt/entity/registry.hpp"

#include "OtComponent.h"
#include "OtEntity.h"


//
//	OtHierarchyComponent
//

class OtHierarchyComponent : public OtComponent {
public:
	// properties
	entt::entity parent{OtNullEntity};
	entt::entity firstChild{OtNullEntity};
	entt::entity lastChild{OtNullEntity};
	entt::entity previousSibling{OtNullEntity};
	entt::entity nextSibling{OtNullEntity};
};
