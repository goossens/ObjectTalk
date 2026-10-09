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
	// access properties
	declareVectorProperty(3, translation, Translation)
	declareVectorProperty(3, rotation, Rotation)
	declareVectorProperty(3, scale, Scale)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtTransformComponentObjectClass>("TransformComponent", OtComponentObjectClass::getMeta());
			metaProperty(Transform, Translation)
			metaProperty(Transform, Rotation)
			metaProperty(Transform, Scale)
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
