//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "tweeny.h"

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

	// control animation
	OtObject start();
	OtObject stop();
	OtObject pause();

	inline bool isRunning() { return running; }
	inline bool isPaused() { return paused; }

	// step forward in animation
	void step(double seconds);

	// update all current animations
	static void update();

	// get type definition
	static OtType getMeta();

private:
	// the tweener object
	tweeny::tween<double> tweener = tweeny::from(0.0);

	// repeat counter
	size_t repeatCounter = 0;

	// control flags
	bool forever = false;
	bool running = false;
	bool paused = false;

	// list of current animations
	static inline std::vector<OtAnimation> animations;
};
