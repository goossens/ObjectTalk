//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "entt/entity/registry.hpp"

#include "OtEntity.h"


//
//	OtScene2
//

class OtScene2 {
public:
	// constructor/destructor
	OtScene2();
	~OtScene2();

	// create a new entity
	OtEntity createEntity();

	// create an entity object fram an entity handle
	OtEntity getEntity(entt::entity entity);

private:
	// registry for all entities and components in ths scene
	entt::registry registry;
};
