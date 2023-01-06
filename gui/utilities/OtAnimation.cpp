//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
	return shared();
}


//
//	OtAnimationClass::to
//

OtObject OtAnimationClass::to(double value) {
	animator.to(value);
	return shared();
}


//
//	OtAnimationClass::via
//

OtObject OtAnimationClass::via(const std::string& easing) {
	animator.via(easing);
	return shared();
}


//
//	OtAnimationClass::during
//

OtObject OtAnimationClass::during(int32_t milliseconds) {
	animator.during(milliseconds);
	return shared();
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

	return shared();
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

	return shared();
}


//
//	OtAnimationClass::seek
//

OtObject OtAnimationClass::seek(float percentage) {
	animator.seek(percentage);
	return shared();
}


//
//	OtAnimationClass::onStep
//

OtObject OtAnimationClass::onStep(OtObject callback) {
	animator.onStep([callback](tweeny::tween<double>& a, double value) {
		OtVM::instance()->callMemberFunction(callback, "__call__", OtObjectCreate(value));
		return false;
	});

	return shared();
}


//
//	OtAnimationClass::step
//

bool OtAnimationClass::step(int32_t deltaMilliseconds) {
	animator.step(deltaMilliseconds);
	return forever || repeatCounter || animator.progress() < 1.0;
}


//
//	OtAnimationClass::getMeta
//

OtType OtAnimationClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtAnimationClass>("Animation", OtGuiClass::getMeta());
		type->set("from", OtFunctionClass::create(&OtAnimationClass::from));
		type->set("to", OtFunctionClass::create(&OtAnimationClass::to));
		type->set("via", OtFunctionClass::create(&OtAnimationClass::via));
		type->set("during", OtFunctionClass::create(&OtAnimationClass::during));
		type->set("repeat", OtFunctionClass::create(&OtAnimationClass::repeat));
		type->set("continuous", OtFunctionClass::create(&OtAnimationClass::continuous));
		type->set("seek", OtFunctionClass::create(&OtAnimationClass::seek));
		type->set("onStep", OtFunctionClass::create(&OtAnimationClass::onStep));
	}

	return type;
}


//
//	OtAnimationClass::create
//

OtAnimation OtAnimationClass::create() {
	OtAnimation animation = std::make_shared<OtAnimationClass>();
	animation->setType(getMeta());
	return animation;
}
