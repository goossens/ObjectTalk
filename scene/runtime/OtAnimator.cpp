//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtAnimator.h"


//
//	OtAnimator::add
//

void OtAnimator::add(OtAnimation animation) {
	// add animation to list
	animations.push_back(animation);
}


//
//	OtAnimator::update
//

void OtAnimator::update() {
	// determine timestep
	float timestep = ImGui::GetIO().DeltaTime;

	// update all animations (and remove them from the list when they are done)
	animations.erase(std::remove_if(animations.begin(), animations.end(), [timestep](OtAnimation animation) {
		return !animation->step(timestep);
	}), animations.end());
}
