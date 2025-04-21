//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtArray.h"
#include "OtFunction.h"
#include "OtString.h"

#include "OtModelComponentObject.h"


//
//	OtModelComponentObjectClass::getAnimations
//

OtObject OtModelComponentObjectClass::getAnimations() {
	OtArray result = OtArray::create();

	if (model->model->isReady()) {
		auto models = model->model->getModel().getAnimationCount();

		for (size_t i = 0; i < models; i++) {
			result->append(OtString::create(model->model->getModel().getAnimationName(i)));
		}
	}

	return result;
}


//
//	OtModelComponentObjectClass::getMeta
//

OtType OtModelComponentObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtType::create<OtModelComponentObjectClass>("ModelComponent", OtObjectClass::getMeta());

		type->set("isReady", OtFunction::create(&OtModelComponentObjectClass::isReady));

		type->set("hasAnimation", OtFunction::create(&OtModelComponentObjectClass::hasAnimation));
		type->set("getAnimationCount", OtFunction::create(&OtModelComponentObjectClass::getAnimationCount));
		type->set("getAnimationName", OtFunction::create(&OtModelComponentObjectClass::getAnimationName));
		type->set("getAnimations", OtFunction::create(&OtModelComponentObjectClass::getAnimations));

		type->set("setAnimation", OtFunction::create(&OtModelComponentObjectClass::setAnimation));
		type->set("fadeToAnimation", OtFunction::create(&OtModelComponentObjectClass::fadeToAnimation));
	}

	return type;
}
