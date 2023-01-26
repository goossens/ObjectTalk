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

#include "entt/entity/registry.hpp"


//
//	Entity
//

using OtEntity = entt::entity;
#define OtEntityNull entt::null


//
//	Functions
//

// see if an entity is valid
static inline bool OtEntityIsNull(OtEntity entity) {
	return entity == OtEntityNull;
}
