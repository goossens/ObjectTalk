//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//	Based on https://github.com/martinfinke/PolyBLEP


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "OtNumbers.h"

#include "OtAudioSettings.h"
#include "OtAudioUtilities.h"
#include "OtOscillator.h"


//
//	OtOscillator::setPulseWidth
//

void OtOscillator::setPulseWidth(float pw) {
	pulseWidth = std::clamp(static_cast<double>(pw), 0.05, 0.95);
}


//
//	OtOscillator::get
//

float OtOscillator::get() {
	double sampleLength = pitch * OtAudioSettings::dt;
	double value;

	switch (waveForm) {
		case WaveForm::sine:
			value = std::sin(std::numbers::pi2 * t);
			break;

		case WaveForm::square: {
			// auto t2 = fraction(t + pulseWidth);
			auto y = t < pulseWidth ? 1.0 : -1.0;
			value = y;
			break;
		}

		case WaveForm::triangle: {
			auto t1 = OtAudioUtilities::fraction<double>(t + 0.25);
			auto t2 = OtAudioUtilities::fraction<double>(t + 0.75);
			auto y = t * 4.0;

			if (y >= 3) {
				y -= 4;

			} else if (y > 1) {
				y = 2 - y;
			}

			value = y + 4 * sampleLength * (OtAudioUtilities::blamp<double>(t1, sampleLength) - OtAudioUtilities::blamp<double>(t2, sampleLength));
			break;
		}

		case WaveForm::sawtooth: {
			auto t1 = OtAudioUtilities::fraction<double>(t + 0.5);
			double y = 2.0 * t1 - 1.0;
			value = y - OtAudioUtilities::blep<double>(t1, sampleLength);
			break;
		}

		default:
			value = 0.0;
	}

	// update the oscillator phase
	t += sampleLength;
	t = OtAudioUtilities::fraction<double>(t);
	return static_cast<float>(value);
}


//
//	OtOscillator::synchronize
//

void OtOscillator::synchronize(float phase) {
	t = OtAudioUtilities::fraction<double>(static_cast<double>(phase));
}
