//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtFunction.h"
#include "OtLog.h"

#include "OtEntityObject.h"
#include "OtSceneObject.h"


//
//	Helper macros
//

#define declareProperty(T, P, N)													\
	inline void set##N(T value) { component().P = value; }							\
	inline T get##N() { return component().P; }

#define metaProperty(T, N)															\
	type->set("set"#N"", OtFunction::create(&Ot##T##ComponentObjectClass::set##N));	\
	type->set("get"#N"", OtFunction::create(&Ot##T##ComponentObjectClass::get##N));


//
//	OtComponentObject
//

class OtComponentObjectClass;
using OtComponentObject = OtObjectPointer<OtComponentObjectClass>;

class OtComponentObjectClass : public OtObjectClass {
public:
	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtComponentObjectClass>("Component", OtObjectClass::getMeta());
			type->set("getScene", OtFunction::create(&OtComponentObjectClass::getScene));
			type->set("getEntity", OtFunction::create(&OtComponentObjectClass::getEntity));
		}

		return type;
	}

protected:
	// set scene and entity reference
	void setEcsReferences(OtScene* s, OtEntity e) {
		scene = s;
		entity = e;
	}

	//
	//	OtMaterialComponentObjectClass::getScene
	//

	// turn scene and entity into ObjectTalk objects
	OtObject getScene() { return OtSceneObject::create(scene); }
	OtObject getEntity() { return OtEntityObject::create(scene, entity); }

	// get reference to component
	template<typename T>
	T& component(OtScene* scene, OtEntity entity) {
		if (!scene) {
			OtLogError("Class [{}] can't be directly instantiated, create component via [Entity] class instead", getTypeName());
		}

		if (!scene->isValidEntity(entity)) {
			OtLogError("Invalid entity reference in [{}] class", getTypeName());
		}

		if (!scene->hasComponent<T>(entity)) {
			OtLogError("Entity does not have a [{}]", getTypeName());
		}

		return scene->getComponent<T>(entity);
	}

	OtScene* scene = nullptr;
	OtEntity entity = OtEntityNull;
};
