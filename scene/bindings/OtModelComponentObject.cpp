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
	if (model->asset->isReady()) {
		return model->asset->getModel().hasAnimation(name);

	} else {
		return false;
	}
}


//
//	OtModelComponentObjectClass::getAnimationCount
//

size_t OtModelComponentObjectClass::getAnimationCount() {
	if (model->asset->isReady()) {
		return model->asset->getModel().getAnimationCount();

	} else {
		return 0;
	}
}


//
//	OtModelComponentObjectClass::getAnimationName
//

std::string OtModelComponentObjectClass::getAnimationName(size_t index) {
	if (model->asset->isReady()) {
		if (index < model->asset->getModel().getAnimationCount()) {
			return model->asset->getModel().getAnimationName(index);

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

	if (model->asset->isReady()) {
		auto models = model->asset->getModel().getAnimationCount();

		for (size_t i = 0; i < models; i++) {
			result->append(OtString::create(model->asset->getModel().getAnimationName(i)));
		}
	}

	return result;
}


//
//	OtModelComponentObjectClass::startAnimation
//

void OtModelComponentObjectClass::startAnimation(const std::string& name) {
	if (model->asset->isReady()) {
		model->asset->getModel().startAnimation(name);

	} else {
		OtLogError("Can't animate model that is not ready");
	}
}


//
//	OtModelComponentObjectClass::fadeToAnimation
//

void OtModelComponentObjectClass::fadeToAnimation(const std::string& name, float duration) {
	if (model->asset->isReady()) {
		model->asset->getModel().fadeToAnimation(name, duration);

	} else {
		OtLogError("Can't animate model that is not ready");
	}
}


//
//	OtModelComponentObjectClass::stopAnimation
//

void OtModelComponentObjectClass::stopAnimation() {
	if (model->asset->isReady()) {
		model->asset->getModel().stopAnimation();
	}
}


//
//	OtModelComponentObjectClass::isAnimating
//

bool OtModelComponentObjectClass::isAnimating() {
	if (model->asset->isReady()) {
		return model->asset->getModel().isAnimating();

	} else {
		return false;
	}
}


//
//	OtModelComponentObjectClass::setAnimationSpeed
//

void OtModelComponentObjectClass::setAnimationSpeed(const std::string& name, float speed) {
	if (model->asset->isReady()) {
		model->asset->getModel().setAnimationSpeed(name, speed);
	}
}


//
//	OtModelComponentObjectClass::getAnimationSpeed
//

float OtModelComponentObjectClass::getAnimationSpeed() {
	if (model->asset->isReady()) {
		return model->asset->getModel().getAnimationSpeed();

	} else {
		return 0.0f;
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

		type->set("setAnimationSpeed", OtFunction::create(&OtModelComponentObjectClass::setAnimationSpeed));
		type->set("getAnimationSpeed", OtFunction::create(&OtModelComponentObjectClass::getAnimationSpeed));
	}

	return type;
}
