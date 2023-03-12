//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <tweeny.h>

#include "OtObject.h"


//
//	OtAnimationClass
//

class OtAnimationClass;
using OtAnimation = OtObjectPointer<OtAnimationClass>;

class OtAnimationClass : public OtObjectClass {
public:
	// animation methods
	OtObject from(double value);
	OtObject to(double value);
	OtObject via(const std::string& easing);
	OtObject during(double milliseconds);

	OtObject repeat(size_t times);
	OtObject continuous();

	OtObject seek(double percentage);

	OtObject onStep(OtObject callback);

	// step forward in animation
	bool step(double seconds);

	// get type definition
	static OtType getMeta();

private:
	// our animator
	tweeny::tween<double> animator = tweeny::from(0.0);

	// repeat counter
	size_t repeatCounter = 0;

	// run forever
	bool forever = false;
};
