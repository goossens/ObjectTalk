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
	inline static OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtCameraComponentObjectClass>("CameraComponent", OtCameraComponentObjectClass::getMeta());

			type->set("setMainCamera", OtFunction::create(&OtCameraComponentObjectClass::setMainCamera));
			type->set("setFOV", OtFunction::create(&OtCameraComponentObjectClass::setFOV));
			type->set("setNearPlane", OtFunction::create(&OtCameraComponentObjectClass::setNearPlane));
			type->set("setFarPlane", OtFunction::create(&OtCameraComponentObjectClass::setFarPlane));

			type->set("isMainCamera", OtFunction::create(&OtCameraComponentObjectClass::isMainCamera));
			type->set("getFOV", OtFunction::create(&OtCameraComponentObjectClass::getFOV));
			type->set("getNearPlane", OtFunction::create(&OtCameraComponentObjectClass::getNearPlane));
			type->set("getFarPlane", OtFunction::create(&OtCameraComponentObjectClass::getFarPlane));
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
