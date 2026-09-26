//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "OtObject.h"

#include "OtScene.h"


//
//	OtSceneObjectClass
//

class OtSceneObjectClass;
using OtSceneObject = OtObjectPointer<OtSceneObjectClass>;

class OtSceneObjectClass : public OtObjectClass {
public:
	// handle entities
	bool hasEntity(const std::string& tag);
	OtObject getEntity(const std::string& tag);
	OtObject addEntity(const std::string& tag);
	void deleteEntity(OtObject object);
	OtObject getRootEntity();

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtSceneObjectClass>;

	OtSceneObjectClass() {
		privateScene = std::make_shared<OtScene>();
		scene = privateScene.get();
	}

	OtSceneObjectClass(OtScene* scene) : scene(scene) {}

private:
	// connection to ECS
	OtScene* scene;

	// privately owned scene for those cases that a scene object was constructed directly
	std::shared_ptr<OtScene> privateScene;
};
