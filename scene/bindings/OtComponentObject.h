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

#include "OtMathModule.h" // IWYU pragma: keep

#include "OtEntityObject.h"
#include "OtSceneObject.h"


//
//	Helper macros
//

#define declareProperty(T, P, N)													\
	inline void set##N(T value) { component().P = value; }							\
	inline T get##N() { return component().P; }

#define declareAssetProperty(P, N)													\
	inline void set##N(const std::string& value) { component().P = value; }			\
	inline std::string get##N() { return component().P.getPath(); }

#define declareVectorProperty(D, P, N)												\
	inline void set##N(size_t count, OtObject* parameters) {						\
		component().P = toVec##D(count, parameters);								\
	}																				\
																					\
	inline glm::vec##D get##N() { return component().P; }

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

	// determine vec2 from calling parameters
	glm::vec2 toVec2(size_t count, OtObject* parameters) {
		if (count == 1) {
			if (parameters[0].isKindOf<OtVec2Class>()) {
				return OtVec2(parameters[0])->operator glm::vec2();

			} else {
				return glm::vec2(parameters[0]->operator float());
			}

		} else if (count == 2) {
				return glm::vec2(
					parameters[0]->operator float(),
					parameters[1]->operator float());

		} else {
			OtLogError("[TransformComponent] expects 1 or 2 arguments (not {})", count);
		}

		return glm::vec2();
	}

	// determine vec3 from calling parameters
	glm::vec3 toVec3(size_t count, OtObject* parameters) {
		if (count == 1) {
			if (parameters[0].isKindOf<OtVec3Class>()) {
				return OtVec3(parameters[0])->operator glm::vec3();

			} else {
				return glm::vec3(parameters[0]->operator float());
			}

		} else if (count == 3) {
				return glm::vec3(
					parameters[0]->operator float(),
					parameters[1]->operator float(),
					parameters[2]->operator float());

		} else {
			OtLogError("[TransformComponent] expects 1 or 3 arguments (not {})", count);
		}

		return glm::vec3();
	}

	// determine vec4 from calling parameters
	glm::vec4 toVec4(size_t count, OtObject* parameters) {
		if (count == 1) {
			if (parameters[0].isKindOf<OtVec4Class>()) {
				return OtVec4(parameters[0])->operator glm::vec4();

			} else {
				return glm::vec4(parameters[0]->operator float());
			}

		} else if (count == 4) {
				return glm::vec4(
					parameters[0]->operator float(),
					parameters[1]->operator float(),
					parameters[2]->operator float(),
					parameters[3]->operator float());

		} else {
			OtLogError("[TransformComponent] expects 1 or 4 arguments (not {})", count);
		}

		return glm::vec4();
	}

	OtScene* scene = nullptr;
	OtEntity entity = OtEntityNull;
};
