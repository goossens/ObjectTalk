//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"

#include "OtCameraComponentObject.h"


//
//	OtCameraComponentObjectClass::getMeta
//

OtType OtCameraComponentObjectClass::getMeta() {
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