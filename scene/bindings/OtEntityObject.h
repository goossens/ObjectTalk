//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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

	// get parent scene
	OtObject getScene();

	// access components
	bool hasModelComponent();
	OtObject getModelComponent();

	bool hasTransformComponent();
	OtObject getTransformComponent();

	bool hasMaterialComponent();
	OtObject getMaterialComponent();

	bool hasMessageComponent();
	OtObject getMessageComponent();

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtEntityObjectClass>;
	OtEntityObjectClass() = default;
	OtEntityObjectClass(OtScene* s, OtEntity e) : scene(s), entity(e) {}

private:
	// connection to ECS
	OtScene* scene;
	OtEntity entity;
};
