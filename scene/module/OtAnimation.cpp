//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtVM.h"

#include "OtAnimation.h"


//
//	OtAnimationClass::from
//

OtObject OtAnimationClass::from(double value) {
	animator = tweeny::from(value);
	return OtObject(this);
}


//
//	OtAnimationClass::to
//

OtObject OtAnimationClass::to(double value) {
	animator.to(value);
	return OtObject(this);
}


//
//	OtAnimationClass::via
//

OtObject OtAnimationClass::via(const std::string& easing) {
	animator.via(easing);
	return OtObject(this);
}


//
//	OtAnimationClass::during
//

OtObject OtAnimationClass::during(double seconds) {
	animator.during((int32_t) (seconds * 1000.0));
	return OtObject(this);
}


//
//	OtAnimationClass::repeat
//

OtObject OtAnimationClass::repeat(size_t times) {
	repeatCounter = times;

	animator.onStep([this](tweeny::tween<double>& tween, double value) {
		this->repeatCounter--;

		if (tween.progress() >= 1.0 && this->repeatCounter) {
			tween.seek(0);
		}

		return false;
	});

	return OtObject(this);
}


//
//	OtAnimationClass::continuous
//

OtObject OtAnimationClass::continuous() {
	forever = true;

	animator.onStep([](tweeny::tween<double>& tween, double value) {
		if (tween.progress() >= 1.0) {
			tween.seek(0);
		}

		return false;
	});

	return OtObject(this);
}


//
//	OtAnimationClass::seek
//

OtObject OtAnimationClass::seek(double percentage) {
	animator.seek((float) percentage);
	return OtObject(this);
}


//
//	OtAnimationClass::onStep
//

OtObject OtAnimationClass::onStep(OtObject callback) {
	animator.onStep([callback](tweeny::tween<double>& a, double value) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(value));
		return false;
	});

	return OtObject(this);
}


//
//	OtAnimationClass::step
//

bool OtAnimationClass::step(double seconds) {
	animator.step((int32_t) (seconds * 1000.0));
	return forever || repeatCounter || animator.progress() < 1.0;
}


//
//	OtAnimationClass::getMeta
//

OtType OtAnimationClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtAnimationClass>("Animation", OtObjectClass::getMeta());
		type->set("from", OtFunction::create(&OtAnimationClass::from));
		type->set("to", OtFunction::create(&OtAnimationClass::to));
		type->set("via", OtFunction::create(&OtAnimationClass::via));
		type->set("during", OtFunction::create(&OtAnimationClass::during));
		type->set("repeat", OtFunction::create(&OtAnimationClass::repeat));
		type->set("continuous", OtFunction::create(&OtAnimationClass::continuous));
		type->set("seek", OtFunction::create(&OtAnimationClass::seek));
		type->set("onStep", OtFunction::create(&OtAnimationClass::onStep));
	}

	return type;
}
