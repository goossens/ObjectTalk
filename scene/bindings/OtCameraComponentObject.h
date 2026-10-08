//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCameraComponent.h"
#include "OtComponentObject.h"


//
//	OtCameraComponentObjectClass
//

class OtCameraComponentObjectClass;
using OtCameraComponentObject = OtObjectPointer<OtCameraComponentObjectClass>;

class OtCameraComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareProperty(bool, mainCamera, MainCamera)
	declareProperty(float, fov, FOV)
	declareProperty(float, nearPlane, NearPlane)
	declareProperty(float, farPlane, FarPlane)

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtCameraComponentObjectClass>("CameraComponent", OtComponentObjectClass::getMeta());
			metaProperty(Camera, MainCamera)
			metaProperty(Camera, FOV)
			metaProperty(Camera, NearPlane)
			metaProperty(Camera, FarPlane)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtCameraComponentObjectClass>;
	OtCameraComponentObjectClass() = default;
	OtCameraComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtCameraComponent& component() { return OtComponentObjectClass::component<OtCameraComponent>(scene, entity); }
};
