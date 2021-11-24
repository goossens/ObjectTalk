//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "guieasing.h"


//
//	Globals
//

struct easingMapping {
	const char* label;
	bx::Easing::Enum key;
};

static easingMapping easingFunctions[] = {
	{ "Linear", bx::Easing::Enum::Linear },
	{ "Step", bx::Easing::Enum::Step },
	{ "Smoothstep", bx::Easing::Enum::SmoothStep },
	{ "Quadratic in", bx::Easing::Enum::InQuad },
	{ "Quadratic out", bx::Easing::Enum::OutQuad },
	{ "Quadratic in and out", bx::Easing::Enum::InOutQuad },
	{ "Cubic in", bx::Easing::Enum::InCubic },
	{ "Cubic out", bx::Easing::Enum::OutCubic },
	{ "Cubic in and out", bx::Easing::Enum::InOutCubic },
	{ "Quartic in", bx::Easing::Enum::InQuart },
	{ "Quartic out", bx::Easing::Enum::OutQuart },
	{ "Quartic in and out", bx::Easing::Enum::InOutQuart },
	{ "Quintic in", bx::Easing::Enum::InQuint },
	{ "Quintic out", bx::Easing::Enum::OutQuint },
	{ "Quintic in and out ", bx::Easing::Enum::InOutQuint },
	{ "Sine in", bx::Easing::Enum::InSine },
	{ "Sine out", bx::Easing::Enum::OutSine },
	{ "Sine in and out", bx::Easing::Enum::InOutSine },
	{ "Circular in", bx::Easing::Enum::InCirc },
	{ "Circular out", bx::Easing::Enum::OutCirc },
	{ "Circular in and out", bx::Easing::Enum::InOutCirc },
	{ "Exponential in", bx::Easing::Enum::InExpo },
	{ "Exponential out", bx::Easing::Enum::OutExpo },
	{ "Exponential in and out", bx::Easing::Enum::InOutExpo },
	{ "Elastic in", bx::Easing::Enum::InElastic },
	{ "Elastic out", bx::Easing::Enum::OutElastic },
	{ "Elastic in and out", bx::Easing::Enum::InOutElastic },
	{ "Back in", bx::Easing::Enum::InBack },
	{ "Back out", bx::Easing::Enum::OutBack },
	{ "Back in and out", bx::Easing::Enum::InOutBack },
	{ "Bounce in", bx::Easing::Enum::InBounce },
	{ "Bounce out", bx::Easing::Enum::OutBounce },
	{ "Bounce in and out", bx::Easing::Enum::InOutBounce }
};


//
//	OtEasingGetCount
//

int OtEasingGetCount() {
	return IM_ARRAYSIZE(easingFunctions);
}


//
//	OtEasingGetLabel
//

std::string OtEasingGetLabel(int index) {
	return easingFunctions[index].label;
}


//
//	OtEasingGetFunction
//

OtEasingFunction OtEasingGetFunction(int index) {
	return bx::getEaseFunc(easingFunctions[index].key);
}


//
//	OtEasingGetFunction
//

OtEasingFunction OtEasingGetFunction(const std::string& name) {
	for (auto entry: easingFunctions) {
		if (name == entry.label) {
			return bx::getEaseFunc(entry.key);
		}
	}

	return nullptr;
}


//
//	ImGui::Easing
//

static bool getter(void* data, int n, const char** out) {
	*out = ((easingMapping*) data)[n].label;
	return true;
};

namespace ImGui {
	bool Easing(const char* label, int* value) {
		return Combo(label, value, getter, easingFunctions, IM_ARRAYSIZE(easingFunctions));
	}
}
