//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//	Based on https://github.com/martinfinke/PolyBLEP


//
//	Include files
//

#include <cmath>

#include "OtAudioSettings.h"
#include "OtAudioUtilities.h"
#include "OtOscillator.h"


//
//	OtOscillator::get
//

float OtOscillator::get(WaveForm waveForm, float pitch, float pulseWidth, float shape) {
	auto dt = pitch * OtAudioSettings::dt;
	float value;

	switch (waveForm) {
		case WaveForm::sine:
			value = std::sin(OtAudioSettings::pi2 * t);
			break;

		case WaveForm::square: {
			auto y = t < pulseWidth ? 1.0f : -1.0f;
			value = y;
			break;
		}

		case WaveForm::triangle: {
			auto t1 = OtAudioUtilities::fraction<float>(t + 0.25f);
			auto t2 = OtAudioUtilities::fraction<float>(t + 0.75f);
			auto y = t * 4.0f;

			if (y >= 3) {
				y -= 4;

			} else if (y > 1) {
				y = 2 - y;
			}

			value = y + 4 * dt * (OtAudioUtilities::blamp<float>(t1, dt) - OtAudioUtilities::blamp<float>(t2, dt));
			break;
		}

		case WaveForm::sawtooth: {
			auto t1 = OtAudioUtilities::fraction<float>(t + 0.5f);
			auto y = 2.0f * t1 - 1.0f;
			value = y - OtAudioUtilities::blep<float>(t1, dt);
			break;
		}

		case WaveForm::wavetable: {
			if (wavetable && wavetable->isValid()) {
				value = wavetable->get(t, shape);

			} else {
				value = 0.0f;
			}

			break;
		}

		default:
			value = 0.0f;
			break;
	}

	// update the oscillator phase
	t += dt;

	if (t > 1.0f) {
		t -= 1.0f;
	}

	return value;
}


//
//	OtOscillator::synchronize
//

void OtOscillator::synchronize(float phase) {
	t = OtAudioUtilities::fraction<float>(phase);
}
