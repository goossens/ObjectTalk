//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	// constructors
	inline OtCircuitControlClass(const char* n, float* v) : name(n), pin(nullptr), value(v) {}
	inline OtCircuitControlClass(const char* n, OtCircuitPin p, float* v) : name(n), pin(p), value(v) {}

	// set properties
	inline OtCircuitControl setRange(float mn, float mx) {
		minValue = mn;
		maxValue = mx;
		return shared_from_this();
	}

	inline OtCircuitControl setIsLogarithmic(bool flag=true) {
		isLogarithmic = flag;
		return shared_from_this();
	}

	inline OtCircuitControl setIsFrequency(bool flag=true) {
		isFrequency = flag;
		return shared_from_this();
	}

	inline OtCircuitControl setLabelFormat(const char* f) {
		format = f;
		return shared_from_this();
	}

	// render the control knob
	inline bool renderKnob() {
		if (pin && pin->isSourceConnected()) {
			ImGui::BeginDisabled();
		}

		auto result = OtUi::knob(name.c_str(), value, minValue, maxValue, format.c_str(), isLogarithmic);

		if (pin && pin->isSourceConnected()) {
			ImGui::EndDisabled();
		}

		return result;
	}

	// get the value for the control (either from the knob or the connected stream)
	inline float getValue(size_t t) {
		if (pin && pin->isSourceConnected()) {
			auto sample = pin->getSample(t);
			return isFrequency ? OtAudioUtilities::cvToFreq(sample) : sample;

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
	bool isLogarithmic = false;
	bool isFrequency = false;
};
