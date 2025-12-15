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

#include "OtOscillator.h"


//
//	Support functions
//

inline double fraction(double n) {
	// get decimal part of floating point number
	double whole;
	return std::modf(n, &whole);
}

inline double square(double n) {
	// calculate square of number
	return n * n;
}


inline double blep(double t, double dt) {
	// determine Band-Limited Step
	// adapted from "Phaseshaping Oscillator Algorithms for Musical SoundSynthesis"
	// by Jari Kleimola, Victor Lazzarini, Joseph Timoney, and Vesa Valimaki
	// http://www.acoustics.hut.fi/publications/papers/smc2010-phaseshaping/

	if (t < dt) {
		return -square(t / dt - 1.0);

	} else if (t > 1.0 - dt) {
		return square((t - 1.0) / dt + 1.0);

	} else {
		return 0.0;
	}
}

inline double blamp(double t, double dt) {
	if (t < dt) {
		t = t / dt - 1.0;
		return -1.0 / 3.0 * square(t) * t;

	} else if (t > 1 - dt) {
		t = (t - 1.0) / dt + 1.0;
		return 1.0 / 3.0 * square(t) * t;

	} else {
		return 0.0;
	}
}


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
	double sampleLength = frequency / sampleRate;
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
			auto t1 = fraction(t + 0.25);
			auto t2 = fraction(t + 0.75);
			auto y = t * 4.0;

			if (y >= 3) {
				y -= 4;

			} else if (y > 1) {
				y = 2 - y;
			}

			value = y + 4 * sampleLength * (blamp(t1, sampleLength) - blamp(t2, sampleLength));
			break;
		}

		case WaveForm::sawtooth: {
			auto t1 = fraction(t + 0.5);
			double y = 2.0 * t1 - 1.0;
			value = y - blep(t1, sampleLength);
			break;
		}
	}

	// update the oscillator phase
	t += sampleLength;
	t = fraction(t);
	return static_cast<float>(value);
}


//
//	OtOscillator::synchronize
//

void OtOscillator::synchronize(float phase) {
	t = fraction(static_cast<double>(phase));
}
