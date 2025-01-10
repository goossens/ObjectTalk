//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtScene.h"


//
//	OtSceneObjectClass
//

class OtSceneObjectClass;
using OtSceneObject = OtObjectPointer<OtSceneObjectClass>;

class OtSceneObjectClass : public OtObjectClass {
public:
	// constructors
	OtSceneObjectClass() = default;
	OtSceneObjectClass(OtScene* s) : scene(s) {}

	// find entities
	bool hasEntity(const std::string& tag);
	OtObject getEntity(const std::string& tag);

	// get type definition
	static OtType getMeta();

private:
	// connection to ECS
	OtScene* scene;
};
