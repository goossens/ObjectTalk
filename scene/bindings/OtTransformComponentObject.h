//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponentObject.h"
#include "OtTransformComponent.h"


//
//	OtTransformComponentObjectClass
//

class OtTransformComponentObjectClass;
using OtTransformComponentObject = OtObjectPointer<OtTransformComponentObjectClass>;

class OtTransformComponentObjectClass : public OtComponentObjectClass {
public:
	// access transform parts
	inline void setTranslation(glm::vec3 translation) { component().translation = translation; }
	inline void setRotation(glm::vec3 rotation) { component().rotation = rotation; }
	inline void setScale(glm::vec3 scale) { component().scale = scale; }

	inline glm::vec3 getTranslation() { return component().translation; }
	inline glm::vec3 getRotation() { return component().rotation; }
	inline glm::vec3 getScale() { return component().scale; }

	// get type definition
	inline static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtTransformComponentObjectClass>("TransformComponent", OtComponentObjectClass::getMeta());

			type->set("setTranslation", OtFunction::create(&OtTransformComponentObjectClass::setTranslation));
			type->set("setRotation", OtFunction::create(&OtTransformComponentObjectClass::setRotation));
			type->set("setScale", OtFunction::create(&OtTransformComponentObjectClass::setScale));

			type->set("getTranslation", OtFunction::create(&OtTransformComponentObjectClass::getTranslation));
			type->set("getRotation", OtFunction::create(&OtTransformComponentObjectClass::getRotation));
			type->set("getScale", OtFunction::create(&OtTransformComponentObjectClass::getScale));
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtTransformComponentObjectClass>;
	OtTransformComponentObjectClass() = default;
	OtTransformComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtTransformComponent& component() { return OtComponentObjectClass::component<OtTransformComponent>(scene, entity); }
};
