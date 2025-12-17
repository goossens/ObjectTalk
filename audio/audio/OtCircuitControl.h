//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtUi.h"

#include "OtAudioUtilities.h"
#include "OtCircuitPin.h"


//
//	OtCircuitControl
//

class OtCircuitControlClass;
using OtCircuitControl = std::shared_ptr<OtCircuitControlClass>;

class OtCircuitControlClass : public std::enable_shared_from_this<OtCircuitControlClass> {
public:
	// constructor
	inline OtCircuitControlClass(const char* n, OtCircuitPin p, float* v) : name(n), pin(p), value(v) {}

	// set properties
	inline OtCircuitControl setRange(float mn, float mx) {
		minValue = mn;
		maxValue = mx;
		return shared_from_this();
	}

	inline OtCircuitControl setIsLogarithmic(bool flag=true) {
		logarithmic = flag;
		return shared_from_this();
	}

	inline OtCircuitControl setIsFrequency(bool flag=true) {
		frequency = flag;
		return shared_from_this();
	}

	inline OtCircuitControl setLabelFormat(const char* f) {
		format = f;
		return shared_from_this();
	}

	// render the control knob
	inline bool renderKnob() {
		if (pin->isSourceConnected()) {
			ImGui::BeginDisabled();
		}

		auto result = OtUi::knob(name.c_str(), value, minValue, maxValue, format.c_str(), logarithmic);

		if (pin->isSourceConnected()) {
			ImGui::EndDisabled();
		}

		return result;
	}

	// get the value for the control (either from the knob or the connected stream)
	inline float getValue(size_t t) {
		if (pin->isSourceConnected()) {
			auto sample = pin->getSignalBuffer()->get(0, t);
			return frequency ? OtCvToFrequency(sample) : sample;

		} else {
			return *value;
		}
	}

private:
	// properties
	std::string name;
	OtCircuitPin pin;
	float* value;
	float minValue = 0.0f;
	float maxValue = 1.0f;
	std::string format = "%.1f";
	bool logarithmic = false;
	bool frequency = false;
};
