//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtScene2.h"


//
//	OtScene2::OtScene2
//

OtScene2::OtScene2() {
}


//
//	OtScene2::~OtScene2
//

OtScene2::~OtScene2() {
}


//
//	OtScene2::createEntity
//

OtEntity OtScene2::createEntity() {
	return OtEntity(&registry, registry.create());
}


//
//	OtScene2::getEntity
//

OtEntity OtScene2::getEntity(entt::entity entity) {
	return OtEntity(&registry, entity);
}
