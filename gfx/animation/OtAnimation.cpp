//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtReal.h"
#include "OtVM.h"

#include "OtAnimation.h"
#include "OtAnimator.h"


//
//	OtAnimationClass::from
//

OtObject OtAnimationClass::from(double value) {
	animator = tweeny::from(value);
	return OtAnimation(this);
}


//
//	OtAnimationClass::to
//

OtObject OtAnimationClass::to(double value) {
	animator.to(value);
	return OtAnimation(this);
}


//
//	OtAnimationClass::via
//

OtObject OtAnimationClass::via(const std::string& easing) {
	animator.via(easing);
	return OtAnimation(this);
}


//
//	OtAnimationClass::during
//

OtObject OtAnimationClass::during(double seconds) {
	animator.during((int32_t) (seconds * 1000.0));
	return OtAnimation(this);
}


//
//	OtAnimationClass::repeat
//

OtObject OtAnimationClass::repeat(size_t times) {
	repeatCounter = times;

	animator.onStep([this](tweeny::tween<double>& tween, double /* value */) {
		this->repeatCounter--;

		if (tween.progress() >= 1.0 && this->repeatCounter) {
			tween.seek(0);
		}

		return false;
	});

	return OtAnimation(this);
}


//
//	OtAnimationClass::continuous
//

OtObject OtAnimationClass::continuous() {
	forever = true;

	animator.onStep([](tweeny::tween<double>& tween, double /* value */) {
		if (tween.progress() >= 1.0) {
			tween.seek(0);
		}

		return false;
	});

	return OtAnimation(this);
}


//
//	OtAnimationClass::seek
//

OtObject OtAnimationClass::seek(double percentage) {
	animator.seek((float) percentage);
	return OtAnimation(this);
}


//
//	OtAnimationClass::onStep
//

OtObject OtAnimationClass::onStep(OtObject callback) {
	animator.onStep([callback](tweeny::tween<double>& /* a */, double value) {
		OtVM::callMemberFunction(callback, "__call__", OtReal::create(value));
		return false;
	});

	return OtAnimation(this);
}


//
//	OtAnimationClass::start
//

OtObject OtAnimationClass::start() {
	running = true;
	paused = false;

	auto animation = OtAnimation(this);
	OtAnimator::add(animation);
	return animation;
}


//
//	OtAnimationClass::stop
//

OtObject OtAnimationClass::stop() {
	running = false;
	paused = true;
	return OtAnimation(this);
}


//
//	OtAnimationClass::pause
//

OtObject OtAnimationClass::pause() {
	paused = true;
	return OtAnimation(this);
}


//
//	OtAnimationClass::step
//

void OtAnimationClass::step(double seconds) {
	animator.step((int32_t) (seconds * 1000.0));
	running = forever || repeatCounter || animator.progress() < 1.0;
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

		type->set("start", OtFunction::create(&OtAnimationClass::start));
		type->set("stop", OtFunction::create(&OtAnimationClass::stop));
		type->set("pause", OtFunction::create(&OtAnimationClass::pause));

		type->set("onStep", OtFunction::create(&OtAnimationClass::onStep));
	}

	return type;
}
