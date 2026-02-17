//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <limits>

#include "OtAudioSettings.h"


//
//	OtPitchYin
//

template<typename T, size_t N>
class OtPitchYin {
public:
	// algorithm developed by A. de Cheveigné and H. Kawahara and published in:
 	// de Cheveigné, A., Kawahara, H. (2002) "YIN, a fundamental frequency estimator for speech and music", J. Acoust. Soc. Am. 111, 1917-1930.
	// see http://recherche.ircam.fr/equipes/pcm/pub/people/cheveign.html
	inline T calculatePitch(const T* inputData) {
		yin[0] = static_cast<T>(1);
		auto runningSum = static_cast<T>(0);

		for (size_t tau = 1; tau < N2; tau++) {
			// compute difference function
			yin[tau] = static_cast<T>(0);

			for (size_t j = 0; j < N2; j++) {
				auto delta = inputData[j] - inputData[j + tau];
				yin[tau] += (delta * delta);
			}

			// compute a cumulative mean normalized difference function
			runningSum += yin[tau];
			yin[tau] *= tau / runningSum;

			if (std::isnan(yin[tau])) {
				yin[tau] = static_cast<T>(1);
			}

			auto period = tau - 3;

			if (tau > 4 && (yin[period] < tolerance) && (yin[period] < yin[period + 1])) {
				return toHz(quadraticPeakPosition(period));
			}
		}

		return toHz(quadraticPeakPosition(minElement()));
	}

private:
	static constexpr T tolerance = static_cast<T>(0.15);
	static constexpr size_t N2 = N / 2;

	T yin[N2];

	inline T quadraticPeakPosition(size_t pos) {
		if (pos == 0 || pos == N2 - 1) {
			return pos;

		} else {
			auto x0 = (pos < 1) ? pos : pos - 1;
			auto x2 = (pos + 1 < N2) ? pos + 1 : pos;

			auto s0 = yin[x0];
			auto s1 = yin[pos];
			auto s2 = yin[x2];

			if (x0 == pos) { return (s1 <= s2) ? pos : x2; }
			else if (x2 == pos) { return (s1 <= s0) ? pos : x0; }
			else { return pos + static_cast<T>(0.5) * (s0 - s2) / (s0 - static_cast<T>(2) * s1 + s2); }
		}
	}

	inline size_t minElement() {
		T minimum = std::numeric_limits<T>::max();
		size_t pos;

		for (size_t i = 0; i < N2; i++) {
			if (yin[i] < minimum) {
				minimum = yin[i];
				pos = i;
			}
		}

		return pos;
	}

	inline T toHz(T input) {
		return (input > 0) ? static_cast<T>(OtAudioSettings::sampleRate) / input : static_cast<T>(0);
	}
};