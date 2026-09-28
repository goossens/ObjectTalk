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
	// access camera parts
	inline void setMainCamera(bool value) { component().mainCamera = value; }
	inline void setFOV(float value) { component().fov = value; }
	inline void setNearPlane(float value) { component().nearPlane = value; }
	inline void setFarPlane(float value) { component().farPlane = value; }

	inline bool isMainCamera() { return component().mainCamera; }
	inline float getFOV() { return component().fov; }
	inline float getNearPlane() { return component().nearPlane; }
	inline float getFarPlane() { return component().farPlane; }

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtCameraComponentObjectClass>;
	OtCameraComponentObjectClass() = default;
	OtCameraComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtCameraComponent& component() { return OtComponentObjectClass::component<OtCameraComponent>(scene, entity); }
};
