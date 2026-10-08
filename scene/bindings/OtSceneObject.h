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
	// load/save scene
	inline void load(const std::string& path) { scene->load(path); }
	inline void save(const std::string& path) { scene->save(path); }

	// handle entities
	inline bool hasEntity(const std::string& tag) {	return scene->hasEntity(tag); }
	OtObject getEntity(const std::string& tag);
	OtObject addEntity(const std::string& tag);
	void deleteEntity(OtObject object);

	// support entity hierarchy
	OtObject getRootEntity();

	// get the post processing settings
	OtObject getPostProcessing();

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

	// helper functions
	OtEntity toEntity(OtObject object);
};
