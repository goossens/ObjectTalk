//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtEntity.h"
#include "OtScene.h"


//
//	OtEntityObjectClass
//

class OtEntityObjectClass;
using OtEntityObject = OtObjectPointer<OtEntityObjectClass>;

class OtEntityObjectClass : public OtObjectClass {
public:
	// link object to ECS
	void linkToECS(OtScene* scene, OtEntity entity);

	// access components
	bool hasTransformComponent();

	OtObject getTransformComponent();

	// get type definition
	static OtType getMeta();

private:
	// connection to ECS
	OtScene* scene;
	OtEntity entity;
};
