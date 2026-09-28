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
	// get parent scene
	OtObject getScene();

	// access entity hierarchy
	inline void moveBefore(OtObject sibling) { scene->moveEntityBefore(toEntity(sibling), entity); }
	inline void moveTo(OtObject parent) { scene->moveEntityTo(toEntity(parent), entity); }

	inline bool hasChildren() { return scene->hasChildren(entity); }
	inline std::string getTag() { return scene->getTag(entity); }

	inline OtObject getParent() { return toObject(scene->getParent(entity)); }
	inline OtObject getFirstChild() { return toObject(scene->getFirstChild(entity)); }
	inline OtObject getLastChild() { return toObject(scene->getLastChild(entity)); }
	inline OtObject getPreviousSibling() { return toObject(scene->getPreviousSibling(entity)); }
	inline OtObject getNextSibling() { return toObject(scene->getNextSibling(entity)); }

	// access components
#define declareComponent(t)				\
	bool has ## t ## Component();		\
	OtObject get ## t ## Component();	\
	OtObject add ## t ## Component();

	declareComponent(Camera)
	declareComponent(Material)
	declareComponent(Message)
	declareComponent(Model)
	declareComponent(Script)
	declareComponent(Transform)

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtEntityObjectClass>;
	OtEntityObjectClass() = default;
	OtEntityObjectClass(OtScene* scene, OtEntity entity) : scene(scene), entity(entity) {}

private:
	// connection to ECS
	friend class OtSceneObjectClass;
	OtScene* scene;
	OtEntity entity;

	// helper functions
	OtEntity toEntity(OtObject object);
	OtObject toObject(OtEntity entity);
};
