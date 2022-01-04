//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <tweeny.h>

#include "gui.h"


//
//	OtAnimationClass
//

class OtAnimationClass;
typedef std::shared_ptr<OtAnimationClass> OtAnimation;

class OtAnimationClass : public OtGuiClass {
public:
	// animation methods
	OtObject from(double value);
	OtObject to(double value);
	OtObject via(const std::string& easing);
	OtObject during(int32_t milliseconds);

	OtObject repeat(size_t times);
	OtObject continuous();

	OtObject seek(float percentage);

	OtObject onStep(OtObject callback);

	// step forward in animation
	bool step(int32_t deltaMilliseconds);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtAnimation create();

private:
	// our animator
	tweeny::tween<double> animator = tweeny::from(0.0);

	// repeat counter
	size_t repeatCounter = 0;

	// run forever
	bool forever = false;
};
