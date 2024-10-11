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

#include "imgui.h"

#include "OtSingleton.h"

#include "OtAnimation.h"


//
//	OtAnimator
//

class OtAnimator : OtSingleton<OtAnimator> {
public:
	// add a new animation
	static inline void add(OtAnimation animation){ instance().animations.push_back(animation); }

	// update all animations
	static inline void update() {
		// determine timestep
		float timestep = ImGui::GetIO().DeltaTime;

		// update all animations (and remove them from the list when they are done)
		auto& animations = instance().animations;

		animations.erase(std::remove_if(animations.begin(), animations.end(), [timestep](OtAnimation animation) {
			return !animation->step(timestep);
		}), animations.end());
	}

private:
	// list of current animations
	std::vector<OtAnimation> animations;
};
