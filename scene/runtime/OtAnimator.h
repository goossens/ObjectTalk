//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtSingleton.h"

#include "OtAnimation.h"


//
//	OtAnimator
//

class OtAnimator : public OtSingleton<OtAnimator> {
public:
	// add a new animation
	void add(OtAnimation animation);

	// update all animations
	void update();

private:
	// list of current animations
	std::vector<OtAnimation> animations;
};
