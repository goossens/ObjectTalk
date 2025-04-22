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
//	OtModelComponentObjectClass::hasAnimation
//

bool OtModelComponentObjectClass::hasAnimation(const std::string& name) {
	if (model->model->isReady()) {
		return model->model->getModel().hasAnimation(name);

	} else {
		return false;
	}
}


//
//	OtModelComponentObjectClass::getAnimationCount
//

size_t OtModelComponentObjectClass::getAnimationCount() {
	if (model->model->isReady()) {
		return model->model->getModel().getAnimationCount();

	} else {
		return 0;
	}
}


//
//	OtModelComponentObjectClass::getAnimationName
//

std::string OtModelComponentObjectClass::getAnimationName(size_t index) {
	if (model->model->isReady()) {
		if (index < model->model->getModel().getAnimationCount()) {
			return model->model->getModel().getAnimationName(index);

		} else {
			OtLogError("Invalid animation index");
		}

	} else {
		OtLogError("Can't access model that is not ready");
	}

	return "";
}


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
//	OtModelComponentObjectClass::startAnimation
//

void OtModelComponentObjectClass::startAnimation(const std::string& name) {
	if (model->model->isReady()) {
		model->model->getModel().startAnimation(name);

	} else {
		OtLogError("Can't animate model that is not ready");
	}
}


//
//	OtModelComponentObjectClass::fadeToAnimation
//

void OtModelComponentObjectClass::fadeToAnimation(const std::string& name, float duration) {
	if (model->model->isReady()) {
		model->model->getModel().fadeToAnimation(name, duration);

	} else {
		OtLogError("Can't animate model that is not ready");
	}
}


//
//	OtModelComponentObjectClass::stopAnimation
//

void OtModelComponentObjectClass::stopAnimation() {
	if (model->model->isReady()) {
		model->model->getModel().stopAnimation();

	} else {
		OtLogError("Model is not ready");
	}
}


//
//	OtModelComponentObjectClass::isAnimating
//

bool OtModelComponentObjectClass::isAnimating() {
	if (model->model->isReady()) {
		return model->model->getModel().isAnimating();

	} else {
		return false;
	}
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

		type->set("startAnimation", OtFunction::create(&OtModelComponentObjectClass::startAnimation));
		type->set("stopAnimation", OtFunction::create(&OtModelComponentObjectClass::stopAnimation));
		type->set("fadeToAnimation", OtFunction::create(&OtModelComponentObjectClass::fadeToAnimation));
		type->set("isAnimating", OtFunction::create(&OtModelComponentObjectClass::isAnimating));
	}

	return type;
}
